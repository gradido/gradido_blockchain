#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H

#include "AddressIndex.h"
#include "Filter.h"
#include "TransactionEntry.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/blockchain/StateChange.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/ByteArray.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/TimepointInterval.h"

#include "rapidjson/document.h"

#include <deque>
#include <map>
#include <vector>

struct grdw_gradido_transaction;

namespace memory {
	class Block;
}

// mYearMonthAddressIndexEntries start with MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE places,
// if a new index is bigger than that, resize with current size + MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE
// deque would be faster with increasing capacity, but slower with access and resize should only be happen all ~8 years (with 100 as MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE)
// and ideally not happen at all, because the gradido node use TransactionIndex per Block, not per entire blockchain
constexpr size_t MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE = 100;

namespace gradido {
	namespace data::compact {
		struct ConfirmedGradidoTx;
	}
	namespace blockchain {
		class AbstractProvider;

	/*!
		* @author einhornimmond
		* @date 2025-12-12
		* @brief store transaction index in memory for fast finding transactions
		*
		* map: uint64 transaction nr, uint32 file cursor
		* map: uint32 address index, uint64 transaction nr
		*/
		class GRADIDOBLOCKCHAIN_EXPORT TransactionsIndex
		{
		public:
			TransactionsIndex() = delete;
			TransactionsIndex(uint32_t communityIdIndex);
			~TransactionsIndex();

			void reset();

			rapidjson::Value serializeToJson(rapidjson::Document::AllocatorType& alloc) const;

			bool addIndicesForTransaction(ConstTransactionEntryPtr transactionEntry, IMutableDictionary<data::PublicKey>& publicKeyDictionary);
			bool addIndicesForTransaction(const data::compact::ConfirmedGradidoTx& compactTx);
			// compactHotTx without coldData, so involved public keys don't contain public keys from sig map
			bool addIndicesForTransaction(const grdw_gradido_transaction* tx, const data::compact::ConfirmedGradidoTx& compactHotTx);

			//! \brief search transaction nrs for search criteria in filter, ignore filter function
			//! \return transaction nrs
			[[deprecated("Use findTransactions with CompactFilter instead.")]]
			std::vector<uint64_t> findTransactions(const Filter& filter, const IDictionary<data::PublicKey>& publicKeyDictionary, uint32_t communityIdIndex) const;

			//! \brief search transaction nrs for search criteria in filter
			//! //! \return transaction nrs
			std::vector<uint64_t> findTransactions(const CompactFilter& filter) const;
			std::vector<uint64_t> getBalanceChangingTxs(uint32_t publicKeyIndex) const;
			std::vector<uint64_t> findTransactionsBalanceChangingForPublicKey(const CompactFilter& filter) const;
			
			StateChange<data::AddressType> getAddressType(data::compact::PublicKeyIndex publicKeyIndex) const;

			inline void updateAddressIndex(const TransactionEntry& transactionEntry, const IDictionary<data::PublicKey>& publicKeyDictionary) const;

			//! count all, ignore pagination
			size_t countTransactions(const CompactFilter& filter) const;
			size_t countBalanceChangingTxs(const CompactFilter& filter) const;

			//! \brief find transaction nrs from specific month and year
			//! \return {0, 0} if nothing found
			std::pair<uint64_t, uint64_t> findTransactionsForMonthYear(date::year_month ym) const;

			inline bool hasTransactionNr(uint64_t transactionNr) const;

			inline uint64_t getMaxTransactionNr() const { return mMaxTransactionNr; }
			inline uint64_t getMinTransactionNr() const { return mMinTransactionNr; }
			inline uint64_t getTransactionsCount() const;

			inline date::year_month getOldestYearMonth() const { return mMinYearMonth; }
			inline date::year_month getNewestYearMonth() const { return mMaxYearMonth; }
			size_t yearMonthToIndex(date::year_month ym) const;
			date::year_month indexToYearMonth(size_t index) const;
			TimepointInterval filteredTimepointInterval(const CompactFilter& filter) const;

			static inline bool canMatchWithoutDeserialize(const Filter& filter);

			inline bool empty() const { return !mMinTransactionNr && !mMaxTransactionNr; }

			//! iterator for caller controlled search
			//! has a additional ability on top of bidirectional_iterator:
			//! - will be marked stopped if Filter returned stop or if at array borders
			//! - it doesn't advance after stopped was triggered, in either direction
			//! - if isStopped is true compare with other iterator (for example end) will succeed, independ from actual index values!
			//! inspiration: https://internalpointers.com/post/writing-custom-iterators-modern-cpp
			struct GRADIDOBLOCKCHAIN_EXPORT SearchIterator
			{
				using iterator_category = std::bidirectional_iterator_tag;
				using difference_type = std::ptrdiff_t; // a month is a iterator step
				using value_type = uint64_t; // transaction nr
				using pointer = const value_type*;  // or also value_type*
				using reference = const value_type&;  // or also value_type&

				SearchIterator(const TransactionsIndex& parent, uint64_t txNr):
					mParent(&parent), mTxNr(txNr), mEntryIndex(0), mMonthYearIndex(parent.mYearMonthAddressIndexEntries.size()), mStopped(true) {
					mMaxMonthYearIndex = mParent->mYearMonthAddressIndexEntries.size();
				}
				//! \param parent TransactionIndex need to be alive while SearchIterator live
				//! \param maxMonthYearIndex first index outside of scope
				SearchIterator(const TransactionsIndex& parent, const CompactFilter& filter, uint32_t entryIndex, uint8_t monthYearIndex)
					: mParent(&parent), mFilter(filter), 
					mTxNr(0), mEntryIndex(entryIndex), mMonthYearIndex(monthYearIndex), mStopped(false)
				{	
					// mMaxMonthYearIndex = mParent->mYearMonthAddressIndexEntries.size();
					mMaxMonthYearIndex = parent.yearMonthToIndex(filter.timepointInterval.getEndDateYM());
					mMinMonthYearIndex = parent.yearMonthToIndex(filter.timepointInterval.getStartDateYM());
					if (monthYearIndex < mMinMonthYearIndex || monthYearIndex > mMaxMonthYearIndex) {
						mStopped = true;
					}
					mTxNr = getCurrentOrNext(isReversed());					
				}
				// explicit SearchIterator(const value_type& v) : mTxNr(v) {};

				value_type operator*() const { return mTxNr; }
				pointer operator->() { return &mTxNr; }

				// Prefix increment
				SearchIterator& operator++() { 
					auto result = advance(isReversed());
					if (result) {
						mTxNr = result;
					}
					return *this; 
				}

				// Prefix decrement
				SearchIterator& operator--() { 
					auto result = advance(!isReversed());
					if (result) {
						mTxNr = result;
					}
					return *this; 
				}

				// Postfix increment
				SearchIterator operator++(int) { SearchIterator tmp = *this; ++(*this); return tmp; }

				// Postfix decrement
				SearchIterator operator--(int) { SearchIterator tmp = *this; --(*this); return tmp; }

				friend bool operator== (const SearchIterator& a, const SearchIterator& b) { 
					return a.isStopped() == b.isStopped();
				};
				friend bool operator!= (const SearchIterator& a, const SearchIterator& b) { return !(a == b); };

				inline bool isStopped() const { return mStopped; }
				inline bool isReversed() const { return mFilter.searchDirection == SearchDirection::DESC; }

			protected:
				//! return transactions nr if valid or 0 if stopped or invalid array indices
				template<typename t>
				uint64_t filterCurrentEntry(size_t currentBucketSize, t* bucket);
				template<typename Iterator>
				uint64_t findNextInRange(Iterator begin, Iterator end);
				uint64_t getCurrentOrNext(bool reversed);
				uint64_t advance(bool reversed);				

			private:				
				const TransactionsIndex* mParent;
				CompactFilter mFilter;
				value_type mTxNr;
				uint32_t mEntryIndex;
				uint8_t mMonthYearIndex;
				uint8_t mMaxMonthYearIndex;
				uint8_t mMinMonthYearIndex;
				bool mStopped;
			};
			
			inline SearchIterator begin(const CompactFilter& filter) const {
				return createSearchIterator(filter, false);
			}
			inline SearchIterator end(const CompactFilter& filter) const {
				return createSearchIterator(filter, true);
			}

		protected:
			//! \param end: mark it as end iterator, end point one after valid entry (or before in case of DESC Search Direction)
			SearchIterator createSearchIterator(const CompactFilter& filter, bool isEnd = false) const;

			size_t yearMonthToIndexUpdateBounds(date::year year, date::month month);
			void transactionNrUpdateBounds(uint64_t transactionNr);
			bool addIndicesForTransaction(
				gradido::data::TransactionType transactionType,
				uint32_t coinCommunityIdIndex,
				date::year year,
				date::month month,
				uint64_t transactionNr,
				const uint32_t* addressIndices,
				uint16_t addressIndiceCount,
				uint8_t isBalanceChanging
			);
			void clearIndexEntries(); 			
			uint64_t				 mMaxTransactionNr;
			uint64_t				 mMinTransactionNr;
			date::year_month mMinYearMonth;
			date::year_month mMaxYearMonth;

			// TODO: add cross group type
			struct TransactionsIndexEntry
			{
				gradido::data::TransactionType	transactionType;
				uint8_t							addressIndiceCount;
				// Bitmask for addressIndices, if bit is set, transaction has changed account balance of addressIndex
				uint8_t							isBalanceChanging;
				uint32_t 						coinCommunityIdIndex;				
				uint64_t						transactionNr;
				uint32_t						addressIndices[4];

				FilterResult isMatchingFilter(const CompactFilter& filter) const;
			};

			struct BalanceTransactionIndexEntry
			{
				gradido::data::TransactionType	transactionType;
				date::month					confirmedMonth;
				date::year					confirmedYear;
				uint32_t						coinCommunityIdIndex;
				uint64_t						transactionNr;

				FilterResult isMatchingFilter(const CompactFilter& filter, date::year_month startYM, date::year_month endYM) const;
			};

			// is used like a cache, even from const
			mutable AddressIndex mAddressIndex;
			// std::map<uint32_t, data::AddressType> mPublicKeyAddressTypes;
			// TODO: check if replace std::list<std::vector> with std::deque make sense (performance side)
			// TODO: check if flatten maps to std::vector<FlatTransactionsIndexEntry> mEntries[month * years] make sense
			// std::map<date::year, std::map<date::month, std::list<std::vector<TransactionsIndexEntry>>>> mYearMonthAddressIndexEntries;
			// first index start from min year month as 0
			std::vector<std::vector<TransactionsIndexEntry>> mYearMonthAddressIndexEntries;
			std::unordered_map<uint32_t, std::vector<BalanceTransactionIndexEntry>> mBalanceChangingTxPerAccountPublicKey;
		};

		void TransactionsIndex::updateAddressIndex(
			const gradido::blockchain::TransactionEntry& transactionEntry,
			const IDictionary<data::PublicKey>& publicKeyDictionary
		) const {
			mAddressIndex.addTransaction(transactionEntry, publicKeyDictionary);
		}

		bool TransactionsIndex::hasTransactionNr(uint64_t transactionNr) const
		{ 
			return transactionNr >= mMinTransactionNr 
				&& transactionNr <= mMaxTransactionNr; 
		}

		uint64_t TransactionsIndex::getTransactionsCount() const
		{
			if (!mMaxTransactionNr && !mMinTransactionNr) return 0;
			return mMaxTransactionNr - mMinTransactionNr + 1;
		}

		bool TransactionsIndex::canMatchWithoutDeserialize(const Filter& filter)
		{
			if (filter.filterFunction) {
				return false;
			}
			return true;
		}
		template<typename t>
		uint64_t TransactionsIndex::SearchIterator::filterCurrentEntry(size_t currentBucketSize, t* bucket)
		{
			if (mStopped || !mParent) { return 0; }
			if (
				mMonthYearIndex > mMaxMonthYearIndex ||
				//mEntryIndex >= mParent->mYearMonthAddressIndexEntries[mMonthYearIndex].size()
				mEntryIndex >= currentBucketSize
				) {
				//throw GradidoNodeInvalidDataException("invalid out of bounds iterator indices");
				mStopped = true;
				return 0;
			}
			//auto entry = mParent->mYearMonthAddressIndexEntries[mMonthYearIndex][mEntryIndex];
			auto entry = (*bucket)[mEntryIndex];
			auto filterResult = entry.isMatchingFilter(mFilter);
			// mParent->mFilterCount++;
			if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
				mStopped = true;
			}
			if ((filterResult & FilterResult::USE) == FilterResult::USE) {
				return entry.transactionNr;
			}
			return 0;
		}
		template<typename Iterator>
		uint64_t TransactionsIndex::SearchIterator::findNextInRange(Iterator begin, Iterator end)
		{
			if (begin == end) {
				return 0;
			}
			for (auto it = begin; it != end; ++it) {
				auto filterResult = it->isMatchingFilter(mFilter);
				if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
					mStopped = true;
				}
				if ((filterResult & FilterResult::USE) == FilterResult::USE) {
					return it->transactionNr;
				}
				if (mStopped) {
					return 0;
				}
			}
			return 0;
		}
	}
}
#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H
