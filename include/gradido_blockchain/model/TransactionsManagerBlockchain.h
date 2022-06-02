#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTIONS_MANAGER_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTIONS_MANAGER_BLOCKCHAIN_H

#include "IGradidoBlockchain.h"

namespace model {
	/*!
		@author einhornimmond
		@date 02.06.2022
		@brief helper class for using transactions manager as blockchain in transactions validation
	*/
	class TransactionsManagerBlockchain: public IGradidoBlockchain
	{
	public:
		TransactionsManagerBlockchain(const std::string& groupAlias);

		std::vector<Poco::SharedPtr<TransactionEntry>> getAllTransactions(std::function<bool(model::TransactionEntry*)> filter = nullptr);
		Poco::SharedPtr<gradido::GradidoBlock> getLastTransaction(std::function<bool(const gradido::GradidoBlock*)> filter = nullptr);
		mpfr_ptr calculateAddressBalance(const std::string& address, const std::string& groupId, Poco::DateTime date);
		proto::gradido::RegisterAddress_AddressType getAddressType(const std::string& address);
		Poco::SharedPtr<TransactionEntry> getTransactionForId(uint64_t transactionId);
		Poco::SharedPtr<TransactionEntry> findLastTransactionForAddress(const std::string& address, const std::string& groupId = "");
		Poco::SharedPtr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true);
		void calculateCreationSum(const std::string& address, int month, int year, Poco::DateTime received, mpfr_ptr sum);
		const std::string& getGroupId() const;
	protected:
		std::string mGroupAlias;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTIONS_MANAGER_BLOCKCHAIN_H