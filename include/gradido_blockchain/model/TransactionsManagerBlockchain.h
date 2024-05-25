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
		std::vector<std::shared_ptr<TransactionEntry>> searchTransactions(
			uint64_t startTransactionNr = 0,
			std::function<FilterResult(model::TransactionEntry*)> filter = nullptr,
			SearchDirection order = SearchDirection::ASC
		);
		std::shared_ptr<gradido::ConfirmedTransaction> getLastTransaction(std::function<bool(const gradido::ConfirmedTransaction*)> filter = nullptr);
		mpfr_ptr calculateAddressBalance(
			const std::string& address,
			const std::string& groupId,
			Timepoint date,
			uint64_t ownTransactionNr
		);
		proto::gradido::RegisterAddress_AddressType getAddressType(const std::string& address);
		std::shared_ptr<TransactionEntry> getTransactionForId(uint64_t transactionId);
		std::shared_ptr<TransactionEntry> findLastTransactionForAddress(const std::string& address, const std::string& groupId = "");
		std::shared_ptr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true);
		std::vector<std::shared_ptr<TransactionEntry>> findTransactions(const std::string& address);
		std::vector<std::shared_ptr<model::TransactionEntry>> findTransactions(const std::string& address, int month, int year);
		void calculateCreationSum(
			const std::string& address,
			int month, int year,
			Timepoint received,
			mpfr_ptr sum
		);
		const std::string& getCommunityId() const;
	protected:

		static std::shared_ptr<model::gradido::ConfirmedTransaction> createBlockFromTransaction(
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