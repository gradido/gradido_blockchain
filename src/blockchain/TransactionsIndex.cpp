#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/TransactionsIndex.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/serialization/toJson.h"

#include "date/date.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

using namespace rapidjson;
using namespace magic_enum;

using std::make_shared;
using std::vector;
using memory::Block;

// control vector size, primarily for cache hit optimization
const size_t TRANSACTION_ENTRY_VECTOR_SIZE = 100;

namespace gradido {
	using data::adapter::toPublicKey;
	using data::AddressType;
	using data::compact::ConfirmedGradidoTx, data::compact::PublicKeyIndex;
	using data::TransactionType;
	using blockchain::Filter;

	namespace blockchain {

		TransactionsIndex::TransactionsIndex()
			: mMaxTransactionNr(0), mMinTransactionNr(0), mMinYearMonth(date::year(0), date::month(0)), mMaxYearMonth(date::year(0), date::month(0))
		{
			mYearMonthAddressIndexEntries.resize(100);
			mFilterCount = 0;
		}

		TransactionsIndex::~TransactionsIndex()
		{
			LOG_F(INFO, "%llu times filter called", mFilterCount);
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
			if (mMinYearMonth.month() == date::month(0) && mMinYearMonth.year() == date::year(0)) {
				mMinYearMonth = date::year_month{ year, month };
			}
			auto newMaxYearMonth = date::year_month{ year, month };
			if (newMaxYearMonth > mMaxYearMonth) {
				mMaxYearMonth = newMaxYearMonth;
			}

			auto index = yearMonthToIndex(year, month);
			assert(mYearMonthAddressIndexEntries.size());
			if (index >= mYearMonthAddressIndexEntries.size()) {
				mYearMonthAddressIndexEntries.resize(mYearMonthAddressIndexEntries.size() * 2);
				if (index >= mYearMonthAddressIndexEntries.size()) {
					throw GradidoNodeInvalidDataException("unexpected jump in transaction index grow");
				}
			}
			return index;
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
			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr || transactionNr < mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}

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
					auto it = mBalanceChangingTxPerAccountPublicKey.find(addressIndex);
					if (it == mBalanceChangingTxPerAccountPublicKey.end()) {
						mBalanceChangingTxPerAccountPublicKey.insert({ addressIndex, {transactionNr} });
					} else {
						it->second.emplace_back(transactionNr);
					}
				}
			}

			return true;
		}

		bool TransactionsIndex::addIndicesForTransaction(ConstTransactionEntryPtr transactionEntry, IMutableDictionary<PublicKey>& publicKeyDictionary)
		{
			auto transactionNr = transactionEntry->getTransactionNr();

			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr || transactionNr < mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}

			uint32_t coinCommunityIndex = transactionEntry->getCoinCommunityIdIndex().has_value()
				? transactionEntry->getCoinCommunityIdIndex().value()
				: transactionEntry->getBlockchainCommunityIdIndex();
			
			const auto& confirmedTransaction = transactionEntry->getConfirmedTransaction();
			auto involvedPublicKeyIndices = confirmedTransaction->getInvolvedAddressIndices();
			std::vector<uint32_t> publicKeyIndices;
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

			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr || transactionNr < mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}
			auto coinCommunityIndex = compactTx.getCoinCommunityId();
			auto involvedPublicKeyIndices = compactTx.getInvolvedAddresses();
			std::vector<uint32_t> publicKeyIndices;
			publicKeyIndices.reserve(publicKeyIndices.size());
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
				coinCommunityIndex ? coinCommunityIndex.value() : compactTx.txCommunityIdIndex,
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

			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr || transactionNr < mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}
			auto coinCommunityIndex = compactHotTx.getCoinCommunityId();
			auto involvedPublicKeyIndices = compactHotTx.getInvolvedAddresses();
			for (int i = 0; i < tx->sig_map_count; i++) {
				involvedPublicKeyIndices.insert(PublicKeyIndex::fromPublicKey(compactHotTx.txCommunityIdIndex, tx->sig_map[i].public_key));
			}
			std::vector<uint32_t> publicKeyIndices;
			publicKeyIndices.reserve(publicKeyIndices.size());
			uint8_t balanceChangingBitMask = 0;
			for (auto& pubKey : involvedPublicKeyIndices) {
				publicKeyIndices.push_back(pubKey.publicKeyIndex);
				if (publicKeyIndices.size() < 8 && compactHotTx.isBalanceUpdated(pubKey)) {
					balanceChangingBitMask |= 1u << (publicKeyIndices.size() - 1);
				}
			}
			auto receivedDate = date::year_month_day{ date::floor<date::days>(compactHotTx.getConfirmedAt().getAsTimepoint()) };
			mAddressIndex.addTransaction(compactHotTx);
			return addIndicesForTransaction(
				compactHotTx.transactionType,
				coinCommunityIndex ? coinCommunityIndex.value() : compactHotTx.txCommunityIdIndex,
				receivedDate.year(),
				receivedDate.month(),
				transactionNr,
				publicKeyIndices.data(),
				static_cast<uint16_t>(publicKeyIndices.size()),
				balanceChangingBitMask
			);
		}
		
		std::vector<uint64_t> TransactionsIndex::findTransactions(const Filter& originalFilter, const IDictionary<PublicKey>& publicKeyDictionary) const
		{
			uint64_t lastBalanceChangedTransactionNr = 0;
			CompactFilter filter(originalFilter, publicKeyDictionary);

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
			/*if (
				lastBalanceChangedTransactionNr
				&& originalFilter.pagination.size == 1 && originalFilter.pagination.page < 2
				&& originalFilter.searchDirection == SearchDirection::DESC
				&& originalFilter.timepointInterval.isEmpty()
				&& !originalFilter.filterFunction
				&& !originalFilter.coinCommunityIdIndex.has_value()
				) {
				if (lastBalanceChangedTransactionNr >= filter.minTransactionNr && lastBalanceChangedTransactionNr <= filter.maxTransactionNr) {
					return { lastBalanceChangedTransactionNr };
				}

			}*/
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
					auto yearMonthIndex = yearMonthToIndex(intervalIt->year(), intervalIt->month());

					if (mYearMonthAddressIndexEntries[yearMonthIndex].empty()) {
						continue;
					}
					for (const auto& entry : mYearMonthAddressIndexEntries[yearMonthIndex]) 
					{
						if (!originalFilter.pagination.hasCapacityLeft(result.size())) {
							return result;
						}
						mFilterCount++;
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
					auto yearMonthIndex = yearMonthToIndex((*it).year(), (*it).month());

					const auto& bucket = mYearMonthAddressIndexEntries[yearMonthIndex];
					if (bucket.empty()) {
						continue;
					}
					for (auto entryIt = bucket.rbegin(); entryIt != bucket.rend(); ++entryIt)
					{
						if (!originalFilter.pagination.hasCapacityLeft(result.size())) {
							return result;
						}
						mFilterCount++;
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
					result.emplace_back(txNr);
				}
				return result;
			}
			return {};
		}

		StateChange<data::AddressType> TransactionsIndex::getAddressType(const memory::ConstBlockPtr& publicKeyPtr, const IDictionary<PublicKey>& publicKeyDictionary) const
		{
			auto publicKeyIndexOptional = publicKeyDictionary.getIndexForData(toPublicKey(publicKeyPtr));
			if (!publicKeyIndexOptional.has_value()) {
				return AddressType::NONE;
			}
			auto addressType = mAddressIndex.getAddressType(publicKeyIndexOptional.value());
			if (AddressType::NONE == addressType) {
				return AddressType::NONE;
			}
			auto txs = mAddressIndex.getAddressTypeChangingTransactions(publicKeyIndexOptional.value());
			if (txs.empty()) {
				return addressType;
			}
			if (txs.size() == 1) {
				return { txs[0], addressType };
			}
			std::sort(txs.begin(), txs.end());
			return { txs.back(), mAddressIndex.getAddressType(publicKeyIndexOptional.value()) };
		}

		size_t TransactionsIndex::countTransactions(const Filter& originalFilter, const IDictionary<PublicKey>& publicKeyDictionary) const
		{
			// prefilter, early exit
			uint64_t lastBalanceChangedTransactionNr = 0;
			CompactFilter filter(originalFilter, publicKeyDictionary);

			if (PublicKeySearchType::MissingIndex == filter.publicKeySearchType) {
				return 0;
			}
			else if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType) {
				lastBalanceChangedTransactionNr = mAddressIndex.lastBalanceChanged(filter.publicKeyIndex);
				if (lastBalanceChangedTransactionNr && (!filter.maxTransactionNr || filter.maxTransactionNr > lastBalanceChangedTransactionNr)) {
					filter.maxTransactionNr = lastBalanceChangedTransactionNr;
				}
			}
			
			if ((filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr) ||
				(filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr)) {
				return 0;
			}

			auto interval = filteredTimepointInterval(originalFilter);
			size_t result = 0;

			for (auto intervalIt = interval.begin(); intervalIt != interval.end(); ++intervalIt) {
				auto yearMonthIndex = yearMonthToIndex(intervalIt->year(), intervalIt->month());

				if (mYearMonthAddressIndexEntries[yearMonthIndex].empty()) {
					continue;
				}
				for (const auto& entry : mYearMonthAddressIndexEntries[yearMonthIndex]) {
					auto filterResult = entry.isMatchingFilter(filter);
					if ((filterResult & FilterResult::USE) == FilterResult::USE) {
						++result;
					}
				}
			}
			return result;
		}

		size_t TransactionsIndex::countBalanceChangingTxs(uint32_t publicKeyIndex) const
		{
			auto it = mBalanceChangingTxPerAccountPublicKey.find(publicKeyIndex);
			if (it != mBalanceChangingTxPerAccountPublicKey.end()) {
				return it->second.size();
			}
			return 0;
		}

		std::pair<uint64_t, uint64_t> TransactionsIndex::findTransactionsForMonthYear(date::year year, date::month month) const
		{
			try {
				auto yearMonthIndex = yearMonthToIndex(year, month);
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

		size_t TransactionsIndex::yearMonthToIndex(date::year year, date::month month) const
		{
			if (mMinYearMonth.month() == date::month(0) && mMinYearMonth.year() == date::year(0)) {
				return 0;
			}
			if (year < mMinYearMonth.year() || (year == mMinYearMonth.year() && month < mMinYearMonth.month())) {
				// TODO: make custom exception for out of bounds
				throw std::out_of_range("TransactionsIndex::yearMonthToIndex called with out of bounds parameters (to low)");
			}
			if (year > mMaxYearMonth.year() || (year == mMaxYearMonth.year() && month > mMaxYearMonth.month())) {
				throw std::out_of_range("TransactionsIndex::yearMonthToIndex called with out of bounds parameters (too high)");
			}
			int years = int(year) - int(mMinYearMonth.year());
			int months = int(unsigned(month)) - int(unsigned(mMinYearMonth.month()));
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
			if (filter.hasCoinCommunityIndex && coinCommunityIdIndex != filter.coinCommunityIdIndex) {
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
					 if ((isBalanceChanging & (uint8_t(1) << iPublicKeyIndex)) && filter.publicKeyIndex == addressIndices[iPublicKeyIndex]) {
						return FilterResult::USE;
					}
				}
				return FilterResult::DISMISS;
			}
			else if (PublicKeySearchType::InvolvedPublicKey == filter.publicKeySearchType) {
				for (int iPublicKeyIndex = 0; iPublicKeyIndex < addressIndiceCount; iPublicKeyIndex++) {
					if (filter.publicKeyIndex == addressIndices[iPublicKeyIndex]) {
						return FilterResult::USE;
					}
				}
				return FilterResult::DISMISS;
			}
			
			return FilterResult::USE;
		}
	}
}