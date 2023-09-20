#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_ENTRY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_ENTRY_H

#include "Poco/Types.h"
#include "Poco/Mutex.h"
#include "Poco/DateTime.h"

#include "gradido_blockchain/model/protobufWrapper/ConfirmedTransaction.h"

#include <vector>

namespace model {

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
			: mTransactionNr(0) {}

		//! \brief init entry object from serialized transaction, deserialize transaction to get infos
		TransactionEntry(std::unique_ptr<std::string> _serializedTransaction);

		TransactionEntry(gradido::ConfirmedTransaction* transaction);

		//! \brief init entry object without indices
		TransactionEntry(uint64_t transactionNr, uint8_t month, uint16_t year, std::string communityId);

		//! \brief operator for sorting by mTransactionNr in ascending order
		bool operator < (const TransactionEntry& b) { return mTransactionNr < b.mTransactionNr; }		

		inline uint64_t getTransactionNr() const { return mTransactionNr; }
		inline const std::string* getSerializedTransaction() const { return mSerializedTransaction.get(); }
		inline uint8_t getMonth() const { return mMonth; }
		inline uint16_t getYear() const { return mYear; }

		inline std::string getCoinCommunityId() const { return mCommunityId; }
		[[deprecated("Replaced by getCoinCommunityId, changed name according to Gradido Apollo implementation")]]
		inline std::string getCoinGroupId() const { return getCoinCommunityId(); }

		static std::string getCoinCommunityId(const gradido::TransactionBody* body);
		[[deprecated("Replaced by getCoinCommunityId, changed name according to Gradido Apollo implementation")]]
		static inline std::string getCoinGroupId(const gradido::TransactionBody* body) { return getCoinCommunityId(body); }

	protected:
		uint64_t mTransactionNr;
		std::unique_ptr<std::string> mSerializedTransaction;
		uint8_t mMonth;
		uint16_t mYear;
		std::string mCommunityId;
		Poco::FastMutex mFastMutex;
	};

};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_ENTRY_H
