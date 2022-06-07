#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTIONS_MANAGER_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTIONS_MANAGER_BLOCKCHAIN_H

#include "IGradidoBlockchain.h"

namespace model {
	/*!
		@author einhornimmond
		@date 02.06.2022
		@brief helper class for using transactions manager as blockchain in transactions validation
	*/
	class GRADIDOBLOCKCHAIN_EXPORT TransactionsManagerBlockchain: public IGradidoBlockchain
	{
	public:
		TransactionsManagerBlockchain(const std::string& groupAlias);
		~TransactionsManagerBlockchain();

		//! \param startTransactionNr only used on ascending order
		std::vector<Poco::SharedPtr<TransactionEntry>> searchTransactions(
			uint64_t startTransactionNr = 0,
			std::function<FilterResult(model::TransactionEntry*)> filter = nullptr,
			SearchDirection order = SearchDirection::ASC
		);
		Poco::SharedPtr<gradido::GradidoBlock> getLastTransaction(std::function<bool(const gradido::GradidoBlock*)> filter = nullptr);
		mpfr_ptr calculateAddressBalance(const std::string& address, const std::string& groupId, Poco::DateTime date);
		proto::gradido::RegisterAddress_AddressType getAddressType(const std::string& address);
		Poco::SharedPtr<TransactionEntry> getTransactionForId(uint64_t transactionId);
		Poco::SharedPtr<TransactionEntry> findLastTransactionForAddress(const std::string& address, const std::string& groupId = "");
		Poco::SharedPtr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true);
		std::vector<Poco::SharedPtr<model::TransactionEntry>> findTransactions(const std::string& address, int month, int year);
		void calculateCreationSum(const std::string& address, int month, int year, Poco::DateTime received, mpfr_ptr sum);
		const std::string& getGroupId() const;
	protected:

		static Poco::SharedPtr<model::gradido::GradidoBlock> createBlockFromTransaction(
			std::shared_ptr<model::gradido::GradidoTransaction> transaction, uint64_t transactionNr
		);

		std::string mGroupAlias;
	};

	class TransactionsCountException : public GradidoBlockchainException
	{
	public:
		explicit TransactionsCountException(const char* what, uint64_t expectedTransactionsCount, uint64_t transactionsCount) noexcept;
		std::string getFullString() const;
	protected:
		uint64_t mExpectedTransactionsCount;
		uint64_t mTransactionsCount;
	};

}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTIONS_MANAGER_BLOCKCHAIN_H