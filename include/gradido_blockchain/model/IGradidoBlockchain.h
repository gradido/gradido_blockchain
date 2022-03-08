#ifndef __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H

//#include "protobufWrapper/GradidoBlock.h"
#include "protobufWrapper/TransactionBase.h"
#include "TransactionEntry.h"
#include "Poco/SharedPtr.h"
#include <vector>

namespace model {

	class IGradidoBlockchain
	{
	public:
		virtual std::vector<Poco::SharedPtr<TransactionEntry>> getAllTransactions(std::function<bool(model::TransactionEntry*)> filter = nullptr) = 0;
		virtual Poco::SharedPtr<gradido::GradidoBlock> getLastTransaction(std::function<bool(const gradido::GradidoBlock*)> filter = nullptr) = 0;
		virtual mpfr_ptr calculateAddressBalance(const std::string& address, uint32_t coinColor, Poco::DateTime date) = 0;
		virtual Poco::SharedPtr<TransactionEntry> getTransactionForId(uint64_t transactionId) = 0;
		virtual Poco::SharedPtr<TransactionEntry> findLastTransactionForAddress(const std::string& address, uint32_t coinColor = 0) = 0;
		virtual Poco::SharedPtr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true) = 0;
		virtual void calculateCreationSum(const std::string& address, int month, int year, Poco::DateTime received, mpfr_ptr sum) = 0;
		virtual uint32_t getGroupDefaultCoinColor() const = 0;

	protected:
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H