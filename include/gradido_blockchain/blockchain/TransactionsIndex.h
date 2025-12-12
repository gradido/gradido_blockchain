#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H

#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/lib/Dictionary.h"

#include "rapidjson/document.h"

#include <vector>
#include <map>

namespace gradido {
	namespace blockchain {

	/*!
		* @author einhornimmond
		* @date 2025-12-12
		* @brief store transaction index in memory for fast finding transactions
		*
		* map: uint64 transaction nr, uint32 file cursor
		* map: uint32 address index, uint64 transaction nr
		*/
		class TransactionsIndex
		{
		public:
			TransactionsIndex(AbstractProvider* blockchainProvider);
			~TransactionsIndex();

			void reset();

			rapidjson::Value serializeToJson(rapidjson::Document::AllocatorType& alloc) const;

			bool addIndicesForTransaction(ConstTransactionEntryPtr transactionEntry);

			bool addIndicesForTransaction(
				gradido::data::TransactionType transactionType,
				uint32_t coinCommunityIdIndex,
				date::year year,
				date::month month,
				uint64_t transactionNr, 
				const uint32_t* addressIndices,
				uint16_t addressIndiceCount
			);

			//! \brief search transaction nrs for search criteria in filter, ignore filter function
			//! \return transaction nrs
			std::vector<uint64_t> findTransactions(const gradido::blockchain::Filter& filter) const;

			//! count all, ignore pagination
			size_t countTransactions(const gradido::blockchain::Filter& filter) const;

			//! \brief find transaction nrs from specific month and year
			//! \return {0, 0} if nothing found
			std::pair<uint64_t, uint64_t> findTransactionsForMonthYear(date::year year, date::month month) const;

			inline bool hasTransactionNr(uint64_t transactionNr) const;

			inline uint64_t getMaxTransactionNr() const { return mMaxTransactionNr; }
			inline uint64_t getMinTransactionNr() const { return mMinTransactionNr; }
			inline uint64_t getTransactionsCount() const;

			date::year_month getOldestYearMonth() const;
			date::year_month getNewestYearMonth() const;
			inline TimepointInterval filteredTimepointInterval(const gradido::blockchain::Filter& filter) const;

		protected:
			void clearIndexEntries(); 			
			uint64_t				 mMaxTransactionNr;
			uint64_t				 mMinTransactionNr;

			struct TransactionsIndexEntry
			{
				uint64_t						transactionNr;
				uint32_t*						addressIndices;
				uint32_t						coinCommunityIdIndex;
				gradido::data::TransactionType	transactionType;
				uint8_t							addressIndiceCount;
				gradido::blockchain::FilterResult isMatchingFilter(
					const gradido::blockchain::Filter& filter, 
					const uint32_t publicKeyIndex,
					gradido::blockchain::AbstractProvider* blockchainProvider
				) const;
			};
			Dictionary mPublicKeyDictionary;
			AbstractProvider* mBlockchainProvider;
			std::map<date::year, std::map<date::month, std::list<TransactionsIndexEntry>>> mYearMonthAddressIndexEntries;
		};

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

		TimepointInterval TransactionsIndex::filteredTimepointInterval(const gradido::blockchain::Filter& filter) const
		{
			TimepointInterval interval(getOldestYearMonth(), getNewestYearMonth());
			if (!filter.timepointInterval.isEmpty()) {
				if (interval.getStartDate() < filter.timepointInterval.getStartDate()) {
					interval.setStartDate(filter.timepointInterval.getStartDate());
				}
				if (interval.getEndDate() > filter.timepointInterval.getEndDate()) {
					interval.setEndDate(std::max(interval.getStartDate(), filter.timepointInterval.getEndDate()));
				}
			}
			return interval;
		}

	}
}
#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_INDEX_H
