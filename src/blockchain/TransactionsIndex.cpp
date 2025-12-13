#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/TransactionsIndex.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/serialization/toJson.h"

#include "loguru/loguru.hpp"

using namespace gradido::blockchain;
using namespace gradido::data;
using namespace rapidjson;

using gradido::blockchain::AbstractProvider;

using std::make_shared;
using memory::Block;

// control vector size, primarily for cache hit optimization
const size_t TRANSACTION_ENTRY_VECTOR_SIZE = 100;

namespace gradido {
	using data::AddressType;

	namespace blockchain {

		TransactionsIndex::TransactionsIndex(AbstractProvider* blockchainProvider)
			: mMaxTransactionNr(0), mMinTransactionNr(0),
			mPublicKeyDictionary("PublicKey"), mBlockchainProvider(blockchainProvider)
		{

		}

		TransactionsIndex::~TransactionsIndex()
		{
			reset();
		}

		void TransactionsIndex::reset()
		{
			clearIndexEntries();
			mPublicKeyDictionary.reset();
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
			for (auto itYear = mYearMonthAddressIndexEntries.begin(); itYear != mYearMonthAddressIndexEntries.end(); itYear++)
			{
				Value yearEntry(kObjectType);
				for (auto itMonth = itYear->second.begin(); itMonth != itYear->second.end(); itMonth++)
				{
					Value monthEntry(kArrayType);
					for (const auto& transactionsIndexEntryVectors : itMonth->second)
					{
						for (const auto& transactionsIndexEntry : transactionsIndexEntryVectors)
						{
							Value entry(kObjectType);
							entry.AddMember("transactionNr", transactionsIndexEntry.transactionNr, alloc);
							entry.AddMember("transactionType", serialization::toJson(transactionsIndexEntry.transactionType, alloc), alloc);
							if (transactionsIndexEntry.coinCommunityIdIndex) {
								entry.AddMember("coinCommunityIdIndex", transactionsIndexEntry.coinCommunityIdIndex, alloc);
							}
							if (transactionsIndexEntry.addressIndiceCount) {
								Value addressIndices(kArrayType);
								for (int i = 0; i < transactionsIndexEntry.addressIndiceCount; i++) {
									addressIndices.PushBack(transactionsIndexEntry.addressIndices[i], alloc);
								}
								entry.AddMember("addressIndices", addressIndices, alloc);
							}
							monthEntry.PushBack(entry, alloc);
						}
					}
					yearEntry.AddMember(serialization::toJson(itMonth->first, alloc), monthEntry, alloc);
				}
				rootJson.AddMember(serialization::toJson(itYear->first, alloc), yearEntry, alloc);
			}
			return rootJson;
		}


		bool TransactionsIndex::addIndicesForTransaction(
			gradido::data::TransactionType transactionType,
			uint32_t coinCommunityIdIndex,
			date::year year,
			date::month month,
			uint64_t transactionNr,
			const uint32_t* addressIndices,
			uint16_t addressIndiceCount
		)
		{
			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr || transactionNr < mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}

			// year
			auto yearIt = mYearMonthAddressIndexEntries.find(year);
			if (yearIt == mYearMonthAddressIndexEntries.end()) {
				auto result = mYearMonthAddressIndexEntries.insert({ year, {} });
				yearIt = result.first;
			}
			// month
			auto monthIt = yearIt->second.find(month);
			if (monthIt == yearIt->second.end()) {
				auto result = yearIt->second.insert({ month, std::list<std::vector<TransactionsIndexEntry>>()});
				monthIt = result.first;
			}
			if (monthIt->second.empty() || monthIt->second.back().size() >= TRANSACTION_ENTRY_VECTOR_SIZE) {
				std::vector<TransactionsIndexEntry> v;
				v.reserve(TRANSACTION_ENTRY_VECTOR_SIZE);
				monthIt->second.push_back(v);
			}
			TransactionsIndexEntry entry;
			entry.transactionNr = transactionNr;
			entry.coinCommunityIdIndex = coinCommunityIdIndex;
			entry.transactionType = transactionType;
			entry.addressIndiceCount = addressIndiceCount;
			if (addressIndiceCount > 256) {
				throw GradidoNodeInvalidDataException("addressIndiceCount is bigger than 256, that cannot be");
			}
			entry.addressIndices = new uint32_t[addressIndiceCount];
			memcpy(entry.addressIndices, addressIndices, sizeof(uint32_t) * addressIndiceCount);

			if (monthIt->second.size() && monthIt->second.back().size() && monthIt->second.back().back().transactionNr >= entry.transactionNr) {
				throw GradidoNodeInvalidDataException("try to add new transaction to block index with same or lesser transaction nr!");
			}
			monthIt->second.back().push_back(entry);

			return true;
		}

		bool TransactionsIndex::addIndicesForTransaction(ConstTransactionEntryPtr transactionEntry)
		{
			auto transactionNr = transactionEntry->getTransactionNr();

			if (transactionNr > mMaxTransactionNr) {
				mMaxTransactionNr = transactionNr;
			}
			if (!mMinTransactionNr || transactionNr < mMinTransactionNr) {
				mMinTransactionNr = transactionNr;
			}

			uint32_t coinCommunityIndex = 0;
			
			if (!transactionEntry->getCoinCommunityId().empty()) {
				coinCommunityIndex = mBlockchainProvider->getCommunityIdIndex(transactionEntry->getCoinCommunityId());
			}
			
			auto involvedPublicKeys = transactionEntry->getConfirmedTransaction()->getInvolvedAddresses();
			std::vector<uint32_t> publicKeyIndices;
			publicKeyIndices.reserve(involvedPublicKeys.size());
			for (auto& pubKey : involvedPublicKeys) {
				auto publicKeyIndex = mPublicKeyDictionary.getOrAddIndexForBinary(pubKey);
				publicKeyIndices.push_back(publicKeyIndex);
			}
			mAddressIndex.addTransaction(*transactionEntry, mPublicKeyDictionary);
			// TODO: fill address types into mPublicKeyAddressTypes, use it for check address request
			return addIndicesForTransaction(
				transactionEntry->getTransactionType(),
				coinCommunityIndex,
				transactionEntry->getYear(),
				transactionEntry->getMonth(),
				transactionNr,
				publicKeyIndices.data(),
				static_cast<uint16_t>(publicKeyIndices.size())
			);

		}
		
		std::vector<uint64_t> TransactionsIndex::findTransactions(const gradido::blockchain::Filter& filter) const
		{
			// prefilter
			if ((filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr) ||
				(filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr)) {
				return {};
			}

			uint32_t publicKeyIndex = 0;
			if (filter.involvedPublicKey && !filter.involvedPublicKey->isEmpty()) {
				if (mPublicKeyDictionary.hasBinary(*filter.involvedPublicKey)) {
					publicKeyIndex = mPublicKeyDictionary.getIndexForBinary(*filter.involvedPublicKey);
				}
				else {
					// if public key not exist, no transaction can match
					return {};
				}
			}

			std::vector<uint64_t> result;
			if (filter.pagination.size) {
				result.reserve(filter.pagination.size);
			}

			auto interval = filteredTimepointInterval(filter);
			int paginationCursor = 0;
			iterateRangeInOrder(interval.begin(), interval.end(), filter.searchDirection,
				[&](const date::year_month& timepoint) -> bool
				{
					// if for a year/month combination no entries exist, return true, so continue the loop
					auto yearIt = mYearMonthAddressIndexEntries.find(timepoint.year());
					if (yearIt == mYearMonthAddressIndexEntries.end()) {
						return true;
					}
					auto monthIt = yearIt->second.find(timepoint.month());
					if (monthIt == yearIt->second.end()) {
						return true;
					}

					iterateRangeInOrder(
						monthIt->second.begin(), monthIt->second.end(), filter.searchDirection,
						[&](const std::vector<TransactionsIndexEntry>& transactionIndexEntriesVector) -> bool
						{
							iterateRangeInOrder(
								transactionIndexEntriesVector.begin(),
								transactionIndexEntriesVector.end(),
								filter.searchDirection,
								[&](const TransactionsIndexEntry& entry)
								{
									auto filterResult = entry.isMatchingFilter(filter, publicKeyIndex, mBlockchainProvider);
									if ((filterResult & FilterResult::USE) == FilterResult::USE) {
										if (paginationCursor >= filter.pagination.skipEntriesCount()) {
											result.push_back(entry.transactionNr);
										}
										paginationCursor++;
									}
									if (!filter.pagination.hasCapacityLeft(result.size()) || (filterResult & FilterResult::STOP) == FilterResult::STOP) {
										return false;
									}
									return true;
								}
							);
							return true;
						}
					);
					return true;
				}
			);
			return result;
		}

		AddressType TransactionsIndex::getAddressType(const memory::Block& publicKey) const
		{
			if (!mPublicKeyDictionary.hasBinary(publicKey)) {
				return AddressType::NONE;
			}
			return mAddressIndex.getAddressType(mPublicKeyDictionary.getIndexForBinary(publicKey));
		}

		size_t TransactionsIndex::countTransactions(const gradido::blockchain::Filter& filter) const
		{
			// prefilter, early exit
			if ((filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr) ||
				(filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr)) {
				return 0;
			}

			uint32_t publicKeyIndex = 0;
			if (filter.involvedPublicKey && !filter.involvedPublicKey->isEmpty()) {
				if (mPublicKeyDictionary.hasBinary(*filter.involvedPublicKey)) {
					publicKeyIndex = mPublicKeyDictionary.getIndexForBinary(*filter.involvedPublicKey);
				}
				else {
					// if public key not exist, no transaction can match
					return 0;
				}
			}

			auto interval = filteredTimepointInterval(filter);
			size_t result = 0;
			iterateRangeInOrder(interval.begin(), interval.end(), filter.searchDirection,
				[&](const date::year_month& intervalIt) -> bool
				{
					auto yearIt = mYearMonthAddressIndexEntries.find(intervalIt.year());
					if (yearIt == mYearMonthAddressIndexEntries.end()) {
						return true;
					}
					auto monthIt = yearIt->second.find(intervalIt.month());
					if (monthIt == yearIt->second.end()) {
						return true;
					}
					iterateRangeInOrder(
						monthIt->second.begin(), monthIt->second.end(), filter.searchDirection,
						[&](const std::vector<TransactionsIndexEntry>& transactionIndexEntriesVector) -> bool
						{
							// iterate over entries in the month
							iterateRangeInOrder(transactionIndexEntriesVector.begin(), transactionIndexEntriesVector.end(), SearchDirection::ASC,
								[&](const TransactionsIndexEntry& entry) -> bool
								{
									auto filterResult = entry.isMatchingFilter(filter, publicKeyIndex, mBlockchainProvider);
									if ((filterResult & FilterResult::USE) == FilterResult::USE) {
										++result;
									}
									return true; // keep going
								}
							);
							return true;
						}
					);
					return true;
				}
			);
			return result;
		}

		std::pair<uint64_t, uint64_t> TransactionsIndex::findTransactionsForMonthYear(date::year year, date::month month) const
		{
			auto yearIt = mYearMonthAddressIndexEntries.find(year);
			if (yearIt == mYearMonthAddressIndexEntries.end()) {
				return { 0, 0 };
			}
			auto monthIt = yearIt->second.find(month);
			if (monthIt == yearIt->second.end()) {
				return { 0, 0 };
			}
			return { monthIt->second.front().front().transactionNr, monthIt->second.back().back().transactionNr};
		}

		date::year_month TransactionsIndex::getOldestYearMonth() const
		{
			if (!mYearMonthAddressIndexEntries.size()) {
				return { date::year(0), date::month(0) };
			}
			auto firstEntry = mYearMonthAddressIndexEntries.begin();
			assert(firstEntry->second.size());
			return { firstEntry->first, firstEntry->second.begin()->first };
		}
		date::year_month TransactionsIndex::getNewestYearMonth() const
		{
			if (!mYearMonthAddressIndexEntries.size()) {
				return { date::year(0), date::month(0) };
			}
			auto lastEntry = std::prev(mYearMonthAddressIndexEntries.end());
			assert(lastEntry->second.size());
			auto lastMonthEntry = std::prev(lastEntry->second.end());
			return { lastEntry->first, lastMonthEntry->first };
		}

		void TransactionsIndex::clearIndexEntries()
		{
			for (const auto& yearBlock : mYearMonthAddressIndexEntries) {
				for (const auto& monthBlock : yearBlock.second) {
					for (const auto& transactionsIndexEntryVectors : monthBlock.second) {
						for (const auto& transactionsIndexEntry : transactionsIndexEntryVectors) {
							delete transactionsIndexEntry.addressIndices;
						}
					}
				}
			}
			mYearMonthAddressIndexEntries.clear();
		}

		FilterResult TransactionsIndex::TransactionsIndexEntry::isMatchingFilter(
			const gradido::blockchain::Filter& filter, 
			const uint32_t publicKeyIndex,
			gradido::blockchain::AbstractProvider* blockchainProvider
		) const
		{
			if (filter.transactionType != TransactionType::NONE
				&& filter.transactionType != transactionType) {
				return FilterResult::DISMISS;
			}
			uint32_t coinCommunityKeyIndex = 0;
			if (!filter.coinCommunityId.empty()) {
				coinCommunityKeyIndex = blockchainProvider->getCommunityIdIndex(filter.coinCommunityId);
			}
			if (coinCommunityKeyIndex && coinCommunityKeyIndex != coinCommunityIdIndex) {
				return FilterResult::DISMISS;
			}
			if (filter.minTransactionNr && filter.minTransactionNr > transactionNr) {
				return FilterResult::DISMISS;
			}
			if (filter.maxTransactionNr && filter.maxTransactionNr < transactionNr) {
				return FilterResult::DISMISS;
			}
			/*uint32_t publicKeyIndex = 0;
			if (filter.involvedPublicKey && !filter.involvedPublicKey->isEmpty()) {
				auto involvedPublicKeyCopy = filter.involvedPublicKey->copyAsString();
				if (publicKeysDictionary.hasString(involvedPublicKeyCopy)) {
					publicKeyIndex = publicKeysDictionary.getIndexForString(involvedPublicKeyCopy);
				}
			}*/
			if (publicKeyIndex) {
				bool found = false;
				for (int iPublicKeyIndices = 0; iPublicKeyIndices < addressIndiceCount; iPublicKeyIndices++) {
					if (publicKeyIndex == addressIndices[iPublicKeyIndices]) {
						found = true;
						break;
					}
				}
				if (!found) {
					return FilterResult::DISMISS;
				}
			}
			return FilterResult::USE;
		}
	}
}