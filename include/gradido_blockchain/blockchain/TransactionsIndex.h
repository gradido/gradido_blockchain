#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H

#include "AddressIndex.h"
#include "Filter.h"
#include "TransactionEntry.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/StateChange.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"

#include "rapidjson/document.h"

#include <deque>
#include <map>
#include <vector>

using DataTypeConverter::monthYearToTimepoint;

namespace memory {
	class Block;
}

namespace gradido {
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
			TransactionsIndex();
			~TransactionsIndex();

			void reset();

			rapidjson::Value serializeToJson(rapidjson::Document::AllocatorType& alloc) const;

			bool addIndicesForTransaction(ConstTransactionEntryPtr transactionEntry, IMutableDictionary<PublicKey>& publicKeyDictionary);
			bool addIndicesForTransaction(const data::compact::ConfirmedGradidoTx& compactTx);
			// compactHotTx without coldData, so involved public keys don't contain public keys from sig map
			bool addIndicesForTransaction(const grdw_gradido_transaction* tx, const data::compact::ConfirmedGradidoTx& compactHotTx);

			//! \brief search transaction nrs for search criteria in filter, ignore filter function
			//! \return transaction nrs
			[[deprecated("Use findTransactions with CompactFilter instead.")]]
			std::vector<uint64_t> findTransactions(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary) const;

			//! \brief search transaction nrs for search criteria in filter
			//! //! \return transaction nrs
			std::vector<uint64_t> findTransactions(const CompactFilter& filter) const;
			std::vector<uint64_t> getBalanceChangingTxs(uint32_t publicKeyIndex) const;
			
			StateChange<data::AddressType> getAddressType(const memory::ConstBlockPtr& publicKeyPtr, const IDictionary<PublicKey>& publicKeyDictionary) const;

			inline void updateAddressIndex(const TransactionEntry& transactionEntry, const IDictionary<PublicKey>& publicKeyDictionary) const;

			//! count all, ignore pagination
			size_t countTransactions(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary) const;
			size_t countBalanceChangingTxs(uint32_t publicKeyIndex) const;

			//! \brief find transaction nrs from specific month and year
			//! \return {0, 0} if nothing found
			std::pair<uint64_t, uint64_t> findTransactionsForMonthYear(date::year year, date::month month) const;

			inline bool hasTransactionNr(uint64_t transactionNr) const;

			inline uint64_t getMaxTransactionNr() const { return mMaxTransactionNr; }
			inline uint64_t getMinTransactionNr() const { return mMinTransactionNr; }
			inline uint64_t getTransactionsCount() const;

			inline date::year_month getOldestYearMonth() const { return mMinYearMonth; }
			inline date::year_month getNewestYearMonth() const { return mMaxYearMonth; }
			size_t yearMonthToIndex(date::year year, date::month month) const;
			date::year_month indexToYearMonth(size_t index) const;
			inline TimepointInterval filteredTimepointInterval(const CompactFilter& filter) const;

			static inline bool canMatchWithoutDeserialize(const Filter& filter);

		protected:
			size_t yearMonthToIndexUpdateBounds(date::year year, date::month month);
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

			struct TransactionsIndexEntry
			{
				uint64_t						transactionNr;
				uint32_t						addressIndices[4];
				uint32_t 						coinCommunityIdIndex;
				gradido::data::TransactionType	transactionType;
				uint8_t							addressIndiceCount;
				// Bitmask for addressIndices, if bit is set, transaction has changed account balance of addressIndex
				uint8_t							isBalanceChanging;
				FilterResult isMatchingFilter(const CompactFilter& filter) const;
			};

			// is used like a cache, even from const
			mutable AddressIndex mAddressIndex;
			mutable size_t mFilterCount;
			// std::map<uint32_t, data::AddressType> mPublicKeyAddressTypes;
			// TODO: check if replace std::list<std::vector> with std::deque make sense (performance side)
			// TODO: check if flatten maps to std::vector<FlatTransactionsIndexEntry> mEntries[month * years] make sense
			// std::map<date::year, std::map<date::month, std::list<std::vector<TransactionsIndexEntry>>>> mYearMonthAddressIndexEntries;
			// fist index start from min year month as 0
			std::vector<std::vector<TransactionsIndexEntry>> mYearMonthAddressIndexEntries;
			std::unordered_map<uint32_t, std::deque<uint64_t>> mBalanceChangingTxPerAccountPublicKey;
		};

		void TransactionsIndex::updateAddressIndex(
			const gradido::blockchain::TransactionEntry& transactionEntry,
			const IDictionary<PublicKey>& publicKeyDictionary
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

		bool TransactionsIndex::canMatchWithoutDeserialize(const Filter& filter)
		{
			if (filter.filterFunction) {
				return false;
			}
			return true;
		}
	}
}
#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H
