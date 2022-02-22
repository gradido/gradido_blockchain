#ifndef __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H

//#include "protobufWrapper/GradidoBlock.h"
#include "protobufWrapper/TransactionBase.h"
#include "Poco/SharedPtr.h"
#include <vector>

namespace model {

	class IGradidoBlockchain
	{
	public:
		virtual std::vector<std::shared_ptr<gradido::GradidoBlock>> getAllTransactions(gradido::TransactionType type) = 0;
		virtual std::shared_ptr<gradido::GradidoBlock> getLastTransaction() = 0;
		virtual std::shared_ptr<gradido::GradidoBlock> getTransactionForId(uint64_t transactionId) = 0;
		virtual Poco::SharedPtr<gradido::GradidoBlock> findByMessageId(MemoryBin* messageId) = 0;
		virtual uint32_t getGroupDefaultCoinColor() const = 0;

	protected:
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H