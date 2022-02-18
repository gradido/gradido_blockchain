#ifndef __GRADIDO_BLOCKCHAIN_MODEL_CROSS_GROUP_TRANSACTION_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_MODEL_CROSS_GROUP_TRANSACTION_BUILDER_H

#include "protobufWrapper/GradidoTransaction.h"

/*!
* @author: einhornimmond
* 
* @date: 28.01.2022
* 
* @brief Used to Build Gradido Cross Group Transactions
*/

class GRADIDOBLOCKCHAIN_EXPORT CrossGroupTransactionBuilder
{
public:
	CrossGroupTransactionBuilder(std::unique_ptr<model::gradido::GradidoTransaction> baseTransaction);
	virtual ~CrossGroupTransactionBuilder();

	// must be called first
	std::unique_ptr<model::gradido::GradidoTransaction> createOutboundTransaction(const std::string& otherGroup);
	// must be called second, after this call, object is empty
	std::unique_ptr<model::gradido::GradidoTransaction> createInboundTransaction(const std::string& otherGroup, MemoryBin* outboundMessageId);
	std::unique_ptr<model::gradido::GradidoTransaction> createInboundTransaction(const std::string& otherGroup);

protected:
	std::unique_ptr<model::gradido::GradidoTransaction> mBaseTransaction;
};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_CROSS_GROUP_TRANSACTION_BUILDER_H