#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_ENTRY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_ENTRY_H

#include "data/Protocol.h"

#include <vector>

namespace gradido {
	namespace v3_3 {

		/*!
		* @author Dario Rekowski
		* @date 2020-03-12
		* @brief container for transaction + index details
		*
		* Used between multiple controller while write to block is pending and while it is cached
		* It contains the serialized transaction and multiple data for fast indexing
		*/
		class GRADIDOBLOCKCHAIN_EXPORT TransactionEntry
		{
		public:
			TransactionEntry()
				: mTransactionNr(0), mMonth(0), mYear(0) {}

			//! \brief init entry object from serialized transaction, deserialize transaction to get infos
			TransactionEntry(memory::ConstBlockPtr serializedTransaction);

			TransactionEntry(const data::ConfirmedTransaction& transaction);

			//! \brief init entry object without indices
			TransactionEntry(uint64_t transactionNr, uint8_t month, uint16_t year, std::string communityId);

			//! \brief operator for sorting by mTransactionNr in ascending order
			bool operator < (const TransactionEntry& b) { return mTransactionNr < b.mTransactionNr; }		

			inline uint64_t getTransactionNr() const { return mTransactionNr; }
			inline memory::ConstBlockPtr getSerializedTransaction() const { return mSerializedTransaction; }
			std::shared_ptr<const data::ConfirmedTransaction> getConfirmedTransaction();
			inline uint8_t getMonth() const { return mMonth; }
			inline uint16_t getYear() const { return mYear; }

			inline std::string getCoinCommunityId() const { return mCommunityId; }
			static std::string getCoinCommunityId(const data::TransactionBody& body);

		protected:
			uint64_t mTransactionNr;
			memory::ConstBlockPtr mSerializedTransaction;
			uint8_t mMonth;
			uint16_t mYear;
			std::string mCommunityId;
			std::mutex mFastMutex;
			std::shared_ptr<const data::ConfirmedTransaction> mConfirmedTransaction;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_ENTRY_H
