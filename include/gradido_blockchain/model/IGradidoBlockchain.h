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
		virtual Poco::SharedPtr<gradido::GradidoBlock> getLastTransaction() = 0;
		virtual Poco::SharedPtr<TransactionEntry> getTransactionForId(uint64_t transactionId) = 0;
		virtual Poco::SharedPtr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true) = 0;
		virtual uint64_t calculateCreationSum(const std::string& address, int month, int year, Poco::DateTime received) = 0;
		virtual uint32_t getGroupDefaultCoinColor() const = 0;

	protected:
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H