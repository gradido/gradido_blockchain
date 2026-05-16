#include "gradido_blockchain_core/data/wire/gradido_transaction.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/TransactionsIndex.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/serialization/toJson.h"
#include "gradido_blockchain/serialization/toJsonString.h"

#include "date/date.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

using namespace rapidjson;
using namespace magic_enum;

using std::sort, std::unique, std::vector;
using memory::Block;

namespace gradido {
	using data::adapter::toPublicKey;
	using data::AddressType;
	using data::compact::ConfirmedGradidoTx, data::compact::PublicKeyIndex;
	using data::TransactionType;
	using blockchain::Filter;

	namespace blockchain {

		TransactionsIndex::TransactionsIndex(uint32_t communityIdIndex)
			: mMaxTransactionNr(0), mMinTransactionNr(0), mMinYearMonth(date::year(0), date::month(0)), mMaxYearMonth(date::year(0), date::month(0)),
			mAddressIndex(communityIdIndex)
		{
			mYearMonthAddressIndexEntries.resize(MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE);
		}

		TransactionsIndex::~TransactionsIndex()
		{
			reset();
		}

		void TransactionsIndex::reset()
		{
			clearIndexEntries();
			mMaxTransactionNr = 0;
			mMinTransactionNr = 0;
		}

		Value TransactionsIndex::serializeToJson(Document::AllocatorType& alloc) const
		{
			if (!mYearMonthAddressIndexEntries.size() && !mMaxTransactionNr && !mMinTransactionNr) {
				// we haven't anything to show
				return Value(kNullType);
			}
			Value rootJson(kObjectType);
			Value yearEntry(kObjectType);
			for (auto monthYearIndex = 0; monthYearIndex < mYearMonthAddressIndexEntries.size(); monthYearIndex++)
			{
				auto monthYear = indexToYearMonth(monthYearIndex);
				if (monthYear.month() <= date::month(1)) {
					if (yearEntry.MemberCount()) {
						rootJson.AddMember(serialization::toJson(monthYear.year(), alloc), yearEntry, alloc);
					}
					yearEntry = Value(kObjectType);
				}
				Value monthEntry(kArrayType);
				for (const auto& itEntry : mYearMonthAddressIndexEntries[monthYearIndex])
				{
					Value entry(kObjectType);
					entry.AddMember("transactionNr", itEntry.transactionNr, alloc);
					entry.AddMember("transactionType", serialization::toJson(itEntry.transactionType, alloc), alloc);
					entry.AddMember("coinCommunityIdIndex", itEntry.coinCommunityIdIndex, alloc);

					if (itEntry.addressIndiceCount) {
						Value addressIndices(kArrayType);
						for (int i = 0; i < itEntry.addressIndiceCount; i++) {
							addressIndices.PushBack(itEntry.addressIndices[i], alloc);
						}
						entry.AddMember("addressIndices", addressIndices, alloc);
					}
					monthEntry.PushBack(entry, alloc);
				}
				if (monthEntry.Size()) {
					yearEntry.AddMember(serialization::toJson(monthYear.month(), alloc), monthEntry, alloc);
				}
			}
			return rootJson;
		}

		size_t TransactionsIndex::yearMonthToIndexUpdateBounds(date::year year, date::month month)
		{
			assert(mYearMonthAddressIndexEntries.size());
			date::year_month currentMinYearMonth = mMinYearMonth;
			if (mMinYearMonth.month() == date::month(0) && mMinYearMonth.year() == date::year(0)) {
				mMinYearMonth = date::year_month{ year, month };
			}

			int years = int(year) - int(mMinYearMonth.year());
			int months = int(unsigned(month)) - int(unsigned(mMinYearMonth.month()));
			auto index = static_cast<size_t>(years * 12 + months);

			if (index >= mYearMonthAddressIndexEntries.size()) {
				mYearMonthAddressIndexEntries.resize(mYearMonthAddressIndexEntries.size() + MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE);
				if (index >= mYearMonthAddressIndexEntries.size()) {
					mMinYearMonth = currentMinYearMonth;
					throw GradidoNodeInvalidDataException("unexpected jump in transaction index grow");
				}
			}

			auto newMaxYearMonth = date::year_month{ year, month };
			if (newMaxYearMonth > mMaxYearMonth) {
				mMaxYearMonth = newMaxYearMonth;
			}

			return index;
		}

		void TransactionsIndex::transactionNrUpdateBounds(uint64_t transactionNr)
		{
			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}
			if (transactionNr < mMinTransactionNr) {
				throw GradidoNodeInvalidDataException("tried to push a transaction with lower nr as mMinTransactionNr");
			}
		}


		bool TransactionsIndex::addIndicesForTransaction(
			gradido::data::TransactionType transactionType,
			uint32_t coinCommunityIdIndex,
			date::year year,
			date::month month,
			uint64_t transactionNr,
			const uint32_t* addressIndices,
			uint16_t addressIndiceCount,
			uint8_t isBalanceChanging
		)
		{
			transactionNrUpdateBounds(transactionNr);

			auto yearMonthIndex = yearMonthToIndexUpdateBounds(year, month);
						
			TransactionsIndexEntry entry{};
			entry.transactionNr = transactionNr;
			entry.coinCommunityIdIndex = coinCommunityIdIndex;
			entry.transactionType = transactionType;
			entry.addressIndiceCount = addressIndiceCount;
			entry.isBalanceChanging = isBalanceChanging;
			if (addressIndiceCount > 4) {
				throw GradidoNotImplementedException("addressIndiceCount is bigger than 4, currently not supported");
			}
			memcpy(entry.addressIndices, addressIndices, sizeof(uint32_t) * addressIndiceCount);
			
			if (mYearMonthAddressIndexEntries[yearMonthIndex].size() && mYearMonthAddressIndexEntries[yearMonthIndex].back().transactionNr >= entry.transactionNr) {
				throw GradidoNodeInvalidDataException("try to add new transaction to block index with same or lesser transaction nr!");
			}
			mYearMonthAddressIndexEntries[yearMonthIndex].emplace_back(entry);

			// add to public key -> balance changing tx 
			if (addressIndiceCount >= 8) {
				LOG_F(WARNING, "more than 8 address indices");
			}
			for (int i = 0; i < addressIndiceCount; i++) {
				if (i >= 8) break;
				if (isBalanceChanging & (uint8_t(1) << i)) {
					auto addressIndex = addressIndices[i];
					BalanceTransactionIndexEntry balanceChangingIndexEntry = {
						.transactionType = transactionType,
						.confirmedMonth = month,
						.confirmedYear = year,
						.coinCommunityIdIndex = coinCommunityIdIndex,
						.transactionNr = transactionNr
					};
					auto it = mBalanceChangingTxPerAccountPublicKey.find(addressIndex);
					if (it == mBalanceChangingTxPerAccountPublicKey.end()) {
						mBalanceChangingTxPerAccountPublicKey.insert({ addressIndex, {balanceChangingIndexEntry} });
					} else {
						it->second.emplace_back(balanceChangingIndexEntry);
					}
				}
			}

			return true;
		}

		bool TransactionsIndex::addIndicesForTransaction(ConstTransactionEntryPtr transactionEntry, IMutableDictionary<PublicKey>& publicKeyDictionary)
		{
			auto transactionNr = transactionEntry->getTransactionNr();
			transactionNrUpdateBounds(transactionNr);

			uint32_t coinCommunityIndex = transactionEntry->getCoinCommunityIdIndex().has_value()
				? transactionEntry->getCoinCommunityIdIndex().value()
				: transactionEntry->getBlockchainCommunityIdIndex();
			
			const auto& confirmedTransaction = transactionEntry->getConfirmedTransaction();
			auto involvedPublicKeyIndices = confirmedTransaction->getInvolvedAddressIndices();
			vector<uint32_t> publicKeyIndices;
			publicKeyIndices.reserve(involvedPublicKeyIndices.size());
			uint8_t balanceChangingBitMask = 0;
			for (auto& pubKey : involvedPublicKeyIndices) {
				auto publicKeyIndex = pubKey.publicKeyIndex;
				publicKeyIndices.push_back(publicKeyIndex);
				if (publicKeyIndices.size() < 8 && confirmedTransaction->isBalanceUpdated(pubKey)) {
					balanceChangingBitMask |= 1u << (publicKeyIndices.size() - 1);
				}
			}
			mAddressIndex.addTransaction(*transactionEntry, publicKeyDictionary);
			// TODO: fill address types into mPublicKeyAddressTypes, use it for check address request
			return addIndicesForTransaction(
				transactionEntry->getTransactionType(),
				coinCommunityIndex,
				transactionEntry->getYear(),
				transactionEntry->getMonth(),
				transactionNr,
				publicKeyIndices.data(),
				static_cast<uint16_t>(publicKeyIndices.size()),
				balanceChangingBitMask
			);

		}

		bool TransactionsIndex::addIndicesForTransaction(const ConfirmedGradidoTx& compactTx)
		{
			auto transactionNr = compactTx.txNr;
			transactionNrUpdateBounds(transactionNr);

			auto coinCommunityIndex = compactTx.coinCommunityIdIndex;
			auto involvedPublicKeyIndices = compactTx.getInvolvedAddresses();
			vector<uint32_t> publicKeyIndices;
			publicKeyIndices.reserve(involvedPublicKeyIndices.size());
			uint8_t balanceChangingBitMask = 0;
			for (auto& pubKey : involvedPublicKeyIndices) {
				publicKeyIndices.push_back(pubKey.publicKeyIndex);
				if (publicKeyIndices.size() < 8 && compactTx.isBalanceUpdated(pubKey)) {
					balanceChangingBitMask |= 1u << (publicKeyIndices.size() - 1);
				}
			}
			auto receivedDate = date::year_month_day{ date::floor<date::days>(compactTx.getConfirmedAt().getAsTimepoint())};
			mAddressIndex.addTransaction(compactTx);
			return addIndicesForTransaction(
				compactTx.transactionType,
				coinCommunityIndex ? coinCommunityIndex : compactTx.txCommunityIdIndex,
				receivedDate.year(),
				receivedDate.month(),
				transactionNr,
				publicKeyIndices.data(),
				static_cast<uint16_t>(publicKeyIndices.size()),
				balanceChangingBitMask
			);
		}

		bool TransactionsIndex::addIndicesForTransaction(const grdw_gradido_transaction* tx, const data::compact::ConfirmedGradidoTx& compactHotTx)
		{
			auto transactionNr = compactHotTx.txNr;
			transactionNrUpdateBounds(transactionNr);

			auto coinCommunityIndex = compactHotTx.coinCommunityIdIndex;
			auto involvedPublicKeyIndices = compactHotTx.getInvolvedAddresses();
			for (int i = 0; i < tx->sig_map_count; i++) {
				involvedPublicKeyIndices.emplace_back(PublicKeyIndex::fromPublicKey(compactHotTx.txCommunityIdIndex, tx->sig_map[i].public_key));
			}
			sort(involvedPublicKeyIndices.begin(), involvedPublicKeyIndices.end());
			auto endIt = unique(involvedPublicKeyIndices.begin(), involvedPublicKeyIndices.end());
			auto beginIt = involvedPublicKeyIndices.begin();
			while (beginIt->empty() && beginIt != endIt) {
				++beginIt;
			}

			vector<uint32_t> publicKeyIndices;
			publicKeyIndices.reserve(involvedPublicKeyIndices.size());
			uint8_t balanceChangingBitMask = 0;
			for (auto it = beginIt; it != endIt; it++) {
				publicKeyIndices.push_back(it->publicKeyIndex);
				if (publicKeyIndices.size() < 8 && compactHotTx.isBalanceUpdated(*it)) {
					balanceChangingBitMask |= 1u << (publicKeyIndices.size() - 1);
				}
			}
			auto receivedDate = date::year_month_day{ date::floor<date::days>(compactHotTx.getConfirmedAt().getAsTimepoint()) };
			mAddressIndex.addTransaction(compactHotTx);
			return addIndicesForTransaction(
				compactHotTx.transactionType,
				coinCommunityIndex ? coinCommunityIndex : compactHotTx.txCommunityIdIndex,
				receivedDate.year(),
				receivedDate.month(),
				transactionNr,
				publicKeyIndices.data(),
				static_cast<uint16_t>(publicKeyIndices.size()),
				balanceChangingBitMask
			);
		}
		
		vector<uint64_t> TransactionsIndex::findTransactions(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary, uint32_t communityIdIndex) const
		{
			return findTransactions(CompactFilter(filter, publicKeyDictionary, communityIdIndex));
		}

		vector<uint64_t> TransactionsIndex::findTransactions(const CompactFilter& originalFilter) const
		{
			uint64_t lastBalanceChangedTransactionNr = 0;
			CompactFilter filter(originalFilter);

			if (PublicKeySearchType::MissingIndex == filter.publicKeySearchType) {
				return {};
			}
			else if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType) {
				lastBalanceChangedTransactionNr = mAddressIndex.lastBalanceChanged(filter.publicKeyIndex);
				if (lastBalanceChangedTransactionNr && (!filter.maxTransactionNr || filter.maxTransactionNr > lastBalanceChangedTransactionNr)) {
					filter.maxTransactionNr = lastBalanceChangedTransactionNr;
				}
			}

			// if user ask for last balance changing transaction
			if (
				lastBalanceChangedTransactionNr
				&& originalFilter.pagination.size == 1 && originalFilter.pagination.page < 2
				&& originalFilter.searchDirection == SearchDirection::DESC
				&& originalFilter.timepointInterval.isEmpty()
				&& originalFilter.coinCommunityIdIndex == 0
				) {
				if (lastBalanceChangedTransactionNr >= filter.minTransactionNr && lastBalanceChangedTransactionNr <= filter.maxTransactionNr) {
					return { lastBalanceChangedTransactionNr };
				}

			}
			// prefilter			
			if ((filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr) ||
				(filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr)) {
				return {};
			}

			if (!originalFilter.timepointInterval.isEmpty() && !originalFilter.timepointInterval.isOverlap({ mMinYearMonth, mMaxYearMonth })) {
				return {};
			}

			std::vector<uint64_t> result;
			if (originalFilter.pagination.size) {
				result.reserve(originalFilter.pagination.size);
			}
			auto interval = filteredTimepointInterval(originalFilter);
			int paginationCursor = 0;
			if (SearchDirection::ASC == filter.searchDirection)
			{
				for (auto intervalIt = interval.begin(); intervalIt != interval.end(); ++intervalIt)
				{
					auto yearMonthIndex = yearMonthToIndex(*intervalIt);
					const auto& entriesOfMonthYear = mYearMonthAddressIndexEntries[yearMonthIndex];

					if (entriesOfMonthYear.empty()) {
						continue;
					}
					if (filter.minTransactionNr) {
						if (entriesOfMonthYear.back().transactionNr < filter.minTransactionNr) {
							continue;
						}
					}
					if (filter.maxTransactionNr) {
						if (entriesOfMonthYear.front().transactionNr > filter.maxTransactionNr) {
							break;
						}
					}
					for (const auto& entry : mYearMonthAddressIndexEntries[yearMonthIndex])
					{
						if (!originalFilter.pagination.hasCapacityLeft(result.size())) {
							return result;
						}
						auto filterResult = entry.isMatchingFilter(filter);
						if ((filterResult & FilterResult::USE) == FilterResult::USE) {
							if (paginationCursor >= originalFilter.pagination.skipEntriesCount()) {
								result.push_back(entry.transactionNr);
							}
							paginationCursor++;
						}
						if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
							return result;
						}
					}
				}
			}
			else if (SearchDirection::DESC == filter.searchDirection) {
				auto rBegin = std::make_reverse_iterator(interval.end());
				auto rEnd = std::make_reverse_iterator(interval.begin());
				for (auto it = rBegin; it != rEnd; ++it)
				{
					auto yearMonthIndex = yearMonthToIndex(*it);
					const auto& entriesOfMonthYear = mYearMonthAddressIndexEntries[yearMonthIndex];
					
					if (entriesOfMonthYear.empty()) {
						continue;
					}
					if (filter.maxTransactionNr) {
						if (entriesOfMonthYear.front().transactionNr > filter.maxTransactionNr) {
							continue;
						}
					}
					if (filter.minTransactionNr) {
						if (entriesOfMonthYear.front().transactionNr < filter.minTransactionNr) {
							break;
						}
					}					
					for (auto entryIt = entriesOfMonthYear.rbegin(); entryIt != entriesOfMonthYear.rend(); ++entryIt)
					{
						if (!originalFilter.pagination.hasCapacityLeft(result.size())) {
							return result;
						}
						auto filterResult = entryIt->isMatchingFilter(filter);
						if ((filterResult & FilterResult::USE) == FilterResult::USE) {
							if (paginationCursor >= originalFilter.pagination.skipEntriesCount()) {
								result.push_back(entryIt->transactionNr);
							}
							paginationCursor++;
						}
						if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
							return result;
						}
					}
				}
			}
			else {
				throw GradidoUnhandledEnum("findTransactions not implemented for", "SearchDirection", enum_name(originalFilter.searchDirection).data());
			}
			return result;
		}

		vector<uint64_t> TransactionsIndex::getBalanceChangingTxs(uint32_t publicKeyIndex) const
		{
			auto it = mBalanceChangingTxPerAccountPublicKey.find(publicKeyIndex);
			if (it != mBalanceChangingTxPerAccountPublicKey.end()) {
				vector<uint64_t> result;
				result.reserve(it->second.size());
				for (const auto& txNr : it->second) {
					result.emplace_back(txNr.transactionNr);
				}
				return result;
			}
			return {};
		}
		vector<uint64_t> TransactionsIndex::findTransactionsBalanceChangingForPublicKey(const CompactFilter& originalFilter) const
		{
			if (PublicKeySearchType::BalanceChangingPublicKey != originalFilter.publicKeySearchType || originalFilter.publicKeyIndex.empty()) {
				throw GradidoNodeInvalidDataException(
					"please call findTransactionsBalanceChangingForPublicKey with PublicKeySearchType = BalanceChangingPublicKey and valid publicKeyIndex"
				);
			}
			auto it = mBalanceChangingTxPerAccountPublicKey.find(originalFilter.publicKeyIndex.publicKeyIndex);
			if (it == mBalanceChangingTxPerAccountPublicKey.end()) {
				return {};
			}
			CompactFilter filter(originalFilter);
			vector<uint64_t> results;
			if (filter.pagination.size) {
				results.reserve(filter.pagination.size);
			}
			filter.timepointInterval = filteredTimepointInterval(filter);
			auto startYm = filter.timepointInterval.getStartDateYM();
			auto endYm = filter.timepointInterval.getEndDateYM();
			return iterateRangeInOrderCollectFiltered
			<std::vector<BalanceTransactionIndexEntry>::const_iterator, std::vector<uint64_t >>
			(it->second.begin(), it->second.end(), filter,
				[&](const BalanceTransactionIndexEntry& entry, uint64_t& id) -> FilterResult
				{
					id = entry.transactionNr;
					return entry.isMatchingFilter(filter, startYm, endYm);
				}
			);
		}

		StateChange<data::AddressType> TransactionsIndex::getAddressType(data::compact::PublicKeyIndex publicKeyIndex) const
		{
			auto addressType = mAddressIndex.getAddressType(publicKeyIndex);
			if (AddressType::NONE == addressType) {
				return AddressType::NONE;
			}
			auto txs = mAddressIndex.getAddressTypeChangingTransactions(publicKeyIndex);
			if (txs.empty()) {
				return addressType;
			}
			if (txs.size() == 1) {
				return { txs[0], addressType };
			}
			std::sort(txs.begin(), txs.end());
			return { txs.back(), mAddressIndex.getAddressType(publicKeyIndex) };
		}

		size_t TransactionsIndex::countTransactions(const CompactFilter& originalFilter) const
		{
			CompactFilter filter(originalFilter);

			if (PublicKeySearchType::MissingIndex == filter.publicKeySearchType) {
				LOG_F(WARNING, "missing index in filter: %s", serialization::toJsonString(originalFilter, true).c_str());
				return 0;
			}
			else if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType) {
				return countBalanceChangingTxs(filter);
			}
			
			if ((filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr) ||
				(filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr)) {
				// filter is out of bounds of TransactionIndex
				return 0;
			}

			auto interval = filteredTimepointInterval(filter);
			size_t result = 0;

			for (auto intervalIt = interval.begin(); intervalIt != interval.end(); ++intervalIt) 
			{
				auto yearMonthIndex = yearMonthToIndex(*intervalIt);

				const auto& entriesOfMonthYear = mYearMonthAddressIndexEntries[yearMonthIndex];
				if (entriesOfMonthYear.empty()) {
					continue;
				}
				if (filter.minTransactionNr) {
					if (entriesOfMonthYear[entriesOfMonthYear.size() - 1].transactionNr < filter.minTransactionNr) {
						continue;
					}
				}
				if (filter.maxTransactionNr) {
					if (entriesOfMonthYear[0].transactionNr > filter.maxTransactionNr) {
						break;
					}
				}
				if (TransactionType::NONE != filter.transactionType || PublicKeySearchType::None != filter.publicKeySearchType || filter.coinCommunityIdIndex) {
					for (const auto& entry : entriesOfMonthYear) {
						auto filterResult = entry.isMatchingFilter(filter);
						if ((filterResult & FilterResult::USE) == FilterResult::USE) {
							++result;
						}
					}
				}
				else {
					auto minTx = entriesOfMonthYear.front().transactionNr;
					auto maxTx = entriesOfMonthYear.back().transactionNr;
					if (filter.minTransactionNr && filter.minTransactionNr > minTx) {
						minTx = filter.minTransactionNr;
					}
					if (filter.maxTransactionNr && filter.maxTransactionNr < maxTx) {
						maxTx = filter.maxTransactionNr;
					}
					if (maxTx >= minTx) {
						result += (maxTx - minTx) + 1;
					}
				}
			}
			return result;
		}

		size_t TransactionsIndex::countBalanceChangingTxs(const CompactFilter& filter) const
		{
			if (PublicKeySearchType::BalanceChangingPublicKey != filter.publicKeySearchType) {
				throw GradidoNodeInvalidDataException("TransactionsIndex::countBalanceChangingTxs called with wrong filter");
			}
			size_t count = 0;
			auto it = mBalanceChangingTxPerAccountPublicKey.find(filter.publicKeyIndex.publicKeyIndex);
			if (it != mBalanceChangingTxPerAccountPublicKey.end()) {
				if (
					!filter.minTransactionNr && 
					!filter.maxTransactionNr && 
					TransactionType::NONE == filter.transactionType && 
					filter.timepointInterval.isEmpty() && 
					!filter.coinCommunityIdIndex
					) {
					return it->second.size();
				}
				CompactFilter filterCopy(filter);
				filterCopy.timepointInterval = filteredTimepointInterval(filter);
				auto startYm = filter.timepointInterval.getStartDateYM();
				auto endYm = filter.timepointInterval.getEndDateYM();
				filterCopy.searchDirection = SearchDirection::ASC;
				for (const auto& entry : it->second) {
					auto result = entry.isMatchingFilter(filterCopy, startYm, endYm);
					if ((FilterResult::USE & result) == FilterResult::USE) {
						++count;
					}
					if ((FilterResult::STOP & result) == FilterResult::STOP) {
						return count;
					}
				}
			}
			return count;
		}

		std::pair<uint64_t, uint64_t> TransactionsIndex::findTransactionsForMonthYear(date::year_month ym) const
		{
			try {
				auto yearMonthIndex = yearMonthToIndex(ym);
				const auto& yearMonthDeque = mYearMonthAddressIndexEntries[yearMonthIndex];
				if (yearMonthDeque.empty()) {
					return { 0,0 };
				}
				return { yearMonthDeque.front().transactionNr, yearMonthDeque.back().transactionNr };
			}
			catch (std::exception& ex) {
				LOG_F(1, "%s", ex.what());
				return { 0,0 };
			}
		}

		size_t TransactionsIndex::yearMonthToIndex(date::year_month ym) const
		{
			if (mMinYearMonth.month() == date::month(0) && mMinYearMonth.year() == date::year(0)) {
				return 0;
			}
			if (ym.year() < mMinYearMonth.year() || (ym.year() == mMinYearMonth.year() && ym.month() < mMinYearMonth.month())) {
				// TODO: make custom exception for out of bounds
				throw std::out_of_range("TransactionsIndex::yearMonthToIndex called with out of bounds parameters (to low)");
			}
			if (ym.year() > mMaxYearMonth.year() || (ym.year() == mMaxYearMonth.year() && ym.month() > mMaxYearMonth.month())) {
				throw std::out_of_range("TransactionsIndex::yearMonthToIndex called with out of bounds parameters (too high)");
			}
			int years = int(ym.year()) - int(mMinYearMonth.year());
			int months = int(unsigned(ym.month())) - int(unsigned(mMinYearMonth.month()));
			return static_cast<size_t>(years * 12 + months);
		}

		date::year_month TransactionsIndex::indexToYearMonth(size_t index) const
		{
			int years = static_cast<int>(floor(index / 12));
			date::year y = mMinYearMonth.year() + date::years(years);
			int month = int(index % 12) + (unsigned)mMinYearMonth.month();
			if (month > 12) {
				y++;
				month -= 12;
			}
			return date::year_month{ y, date::month(month) };
		}

		TimepointInterval TransactionsIndex::filteredTimepointInterval(const CompactFilter& filter) const
		{
			TimepointInterval interval(getOldestYearMonth(), getNewestYearMonth());
			if (filter.timepointInterval.isEmpty() || !filter.timepointInterval.isOverlap(interval)) {
				return interval;
			}
			return {
				std::max(interval.getStartDate(), filter.timepointInterval.getStartDate()),
				std::min(interval.getEndDate(), filter.timepointInterval.getEndDate())
			};
		}

		void TransactionsIndex::clearIndexEntries()
		{
			for (auto& entries : mYearMonthAddressIndexEntries) {
				entries.clear();
			}
		}

		FilterResult TransactionsIndex::TransactionsIndexEntry::isMatchingFilter(const CompactFilter& filter) const
		{
			if (filter.transactionType != TransactionType::NONE
				&& filter.transactionType != transactionType) {
				return FilterResult::DISMISS;
			}
			if (filter.coinCommunityIdIndex && coinCommunityIdIndex != filter.coinCommunityIdIndex) {
				return FilterResult::DISMISS;
			}
			if (filter.minTransactionNr && filter.minTransactionNr > transactionNr) {
				if (filter.searchDirection == SearchDirection::DESC) {
					return FilterResult::STOP;
				}
				return FilterResult::DISMISS;
			}
			if (filter.maxTransactionNr && filter.maxTransactionNr < transactionNr) {
				if (filter.searchDirection == SearchDirection::ASC) {
					return FilterResult::STOP;
				}
				return FilterResult::DISMISS;
			}
			if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType) {
				for (int iPublicKeyIndex = 0; iPublicKeyIndex < addressIndiceCount; iPublicKeyIndex++) {
					if ((isBalanceChanging & (uint8_t(1) << iPublicKeyIndex)) && filter.publicKeyIndex.publicKeyIndex == addressIndices[iPublicKeyIndex]) {
						return FilterResult::USE;
					}
				}
				return FilterResult::DISMISS;
			}
			else if (PublicKeySearchType::InvolvedPublicKey == filter.publicKeySearchType) {
				for (int iPublicKeyIndex = 0; iPublicKeyIndex < addressIndiceCount; iPublicKeyIndex++) {
					if (filter.publicKeyIndex.publicKeyIndex == addressIndices[iPublicKeyIndex]) {
						return FilterResult::USE;
					}
				}
				return FilterResult::DISMISS;
			}

			return FilterResult::USE;
		}

		FilterResult TransactionsIndex::BalanceTransactionIndexEntry::isMatchingFilter(const CompactFilter& filter, date::year_month startYM, date::year_month endYM) const
		{
			if (filter.transactionType != TransactionType::NONE
				&& filter.transactionType != transactionType) {
				return FilterResult::DISMISS;
			}
			if (startYM.year() != date::year(0) && startYM.month() != date::month(0) &&
				  endYM.year() != date::year(0) && endYM.month() != date::month(0))
			{
				if (confirmedYear < startYM.year() || (confirmedYear == startYM.year() && confirmedMonth < startYM.month())) {
					if (SearchDirection::DESC == filter.searchDirection) {
						return FilterResult::STOP;
					}
					return FilterResult::DISMISS; 
				}
				if (confirmedYear > endYM.year() || (confirmedYear == endYM.year() && confirmedMonth > endYM.month())) {
					if (SearchDirection::ASC == filter.searchDirection) {
						return FilterResult::STOP;
					}
					return FilterResult::DISMISS; 
				}
			}
			if (filter.coinCommunityIdIndex && coinCommunityIdIndex != filter.coinCommunityIdIndex) {
				return FilterResult::DISMISS;
			}
			if (filter.maxTransactionNr && filter.maxTransactionNr < transactionNr) {
				if (filter.searchDirection == SearchDirection::ASC) {
					return FilterResult::STOP;
				}
				return FilterResult::DISMISS;
			}
			if (filter.minTransactionNr && filter.minTransactionNr > transactionNr) {
				if (filter.searchDirection == SearchDirection::DESC) {
					return FilterResult::STOP;
				}
				return FilterResult::DISMISS;
			}

			return FilterResult::USE;
		}

		uint64_t TransactionsIndex::SearchIterator::getCurrentOrNext(bool reversed)
		{
			auto bucket = &mParent->mYearMonthAddressIndexEntries[mMonthYearIndex];
			auto bucketSize = bucket->size();
			// of out of array bounds so we stop here
			if (mMonthYearIndex > mMaxMonthYearIndex ||
				mEntryIndex >= bucketSize) {
				mStopped = true;
				return 0;
			}
			auto result = filterCurrentEntry(bucketSize, bucket);
			if (!result) {
				result = advance(reversed);
			}
			return result;
		}

		// Transaction Index Iterator
		uint64_t TransactionsIndex::SearchIterator::advance(bool reversed)
		{
			if (mStopped || !mParent) {
				return 0;
			}
			if (mMonthYearIndex > mMaxMonthYearIndex) {
				throw GradidoNodeInvalidDataException("TransactionsIndex::SearchIterator::advance dont't work like expected, month year index is outside of allowed range");
			}
			auto bucket = &mParent->mYearMonthAddressIndexEntries[mMonthYearIndex];
			auto bucketSize = bucket->size();
			// ASC		
			if (!reversed) {
				do {		
					while (bucketSize - 1 <= mEntryIndex) {
						if (mMonthYearIndex == mMaxMonthYearIndex) {
							mStopped = true;
							return 0;
						}
						// skip holes
						while (!mParent->mYearMonthAddressIndexEntries[++mMonthYearIndex].size()) {
							if (mMonthYearIndex == mMaxMonthYearIndex) {
								mStopped = true;
								return 0;
							}
						};
						bucket = &mParent->mYearMonthAddressIndexEntries[mMonthYearIndex];
						bucketSize = bucket->size();
						mEntryIndex = 0;
						
						if (bucketSize) {
							auto result = filterCurrentEntry(bucketSize, bucket);
							if (result) {
								return result;
							}
							if (mStopped) {
								return 0;
							}
						}
					}
					
					mEntryIndex++;
					auto result = filterCurrentEntry(bucketSize, bucket);
					if (result) {
						return result; 
					}
				} while (!mStopped);
			}
			else {
				// DESC
				do {
					while (!mEntryIndex) {
						if (mMonthYearIndex == mMinMonthYearIndex) {
							mStopped = true;
							return 0;
						}
						// skip 
						if (!mMonthYearIndex) {
							throw GradidoNodeInvalidDataException("mMonthYearIndex is unecpected on 0");
						}
						while (!mParent->mYearMonthAddressIndexEntries[--mMonthYearIndex].size()) {
							if (mMonthYearIndex == mMinMonthYearIndex) {
								mStopped = true;
								return 0;
							}
						};
						bucket = &mParent->mYearMonthAddressIndexEntries[mMonthYearIndex];
						bucketSize = bucket->size();
						mEntryIndex = bucketSize;
					}
					auto rstartIt = bucket->rbegin();
					for (int i = 0; i < bucket->size() - mEntryIndex; i++) {
						rstartIt++;
					}
					auto result = findNextInRange(rstartIt, bucket->rend());
					if (result) {
						mEntryIndex = result - bucket->front().transactionNr;
						return result;
					}
					else {
						mEntryIndex = 0;
					}
				} while (!mStopped);
			}

			mStopped = true;
			return 0;
		}

		TransactionsIndex::SearchIterator TransactionsIndex::createSearchIterator(const CompactFilter& originalFilter, bool isEnd/* = false */) const
		{
			if (empty()) {
				return SearchIterator(*this, mMaxTransactionNr + 1);
			}
			uint64_t lastBalanceChangedTransactionNr = 0;
			CompactFilter filter(originalFilter);

			if (!originalFilter.timepointInterval.isEmpty() && !originalFilter.timepointInterval.isOverlap({ mMinYearMonth, mMaxYearMonth })) {
				return SearchIterator(*this, mMaxTransactionNr + 1);
			}
			if ((filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr) ||
				(filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr)) {
				return SearchIterator(*this, mMaxTransactionNr + 1);
			}

			if (PublicKeySearchType::MissingIndex == filter.publicKeySearchType) {
				throw GradidoNodeInvalidDataException("invalid filter, PublicKeySearchType is MissingIndex");
			}
			else if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType) {
				lastBalanceChangedTransactionNr = mAddressIndex.lastBalanceChanged(filter.publicKeyIndex);
				if (lastBalanceChangedTransactionNr && (!filter.maxTransactionNr || filter.maxTransactionNr > lastBalanceChangedTransactionNr)) {
					filter.maxTransactionNr = lastBalanceChangedTransactionNr;
				}
			}
			
			filter.timepointInterval = filteredTimepointInterval(filter);
			size_t monthYearIndex = 0;
			size_t entryIndex = 0;
			// begin with last entry
			if (SearchDirection::DESC == originalFilter.searchDirection && !isEnd ||
				SearchDirection::ASC == originalFilter.searchDirection && isEnd) {
				monthYearIndex = yearMonthToIndex(filter.timepointInterval.getEndDateYM());
				assert(monthYearIndex < mYearMonthAddressIndexEntries.size());
				// skip some entries when knowing max transaction nr
				if (filter.maxTransactionNr) {
					while (
						mYearMonthAddressIndexEntries[monthYearIndex].front().transactionNr > filter.maxTransactionNr
						&& monthYearIndex
						) {
						--monthYearIndex;
					}
					auto firstTx = mYearMonthAddressIndexEntries[monthYearIndex].front().transactionNr;
					assert(firstTx <= filter.maxTransactionNr);
					entryIndex = filter.maxTransactionNr - firstTx;
				}
				else {
					entryIndex = mYearMonthAddressIndexEntries[monthYearIndex].size() - 1;
				}
			}
			// begin with first entry
			else {
				monthYearIndex = yearMonthToIndex(filter.timepointInterval.getStartDateYM());
				assert(monthYearIndex < mYearMonthAddressIndexEntries.size());
				entryIndex = 0;
				if (filter.minTransactionNr) {
					while (mYearMonthAddressIndexEntries[monthYearIndex].back().transactionNr < filter.minTransactionNr 
						&& monthYearIndex < mYearMonthAddressIndexEntries.size() - 1) {
						++monthYearIndex;
					}
					auto firstTx = mYearMonthAddressIndexEntries[monthYearIndex].front().transactionNr;
					assert(firstTx <= filter.minTransactionNr);
					entryIndex = filter.minTransactionNr - firstTx;
				}
			}
			auto it = SearchIterator(*this, filter, entryIndex, monthYearIndex);
			if (isEnd) {
				// set at past end
				++it;
			}
			return it;
		}
	}
}