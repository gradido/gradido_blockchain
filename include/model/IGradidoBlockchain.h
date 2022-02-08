#ifndef __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H

//#include "protobufWrapper/GradidoBlock.h"
#include "protobufWrapper/TransactionBase.h"

#include <vector>

namespace model {

	class IGradidoBlockchain
	{
	public:
		virtual std::vector<std::shared_ptr<gradido::GradidoBlock>> getAllTransactions(gradido::TransactionType type) = 0;
		virtual std::shared_ptr<gradido::GradidoBlock> getLastTransaction() = 0;

	protected:
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H