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

class CrossGroupTransactionBuilder
{
public:
	CrossGroupTransactionBuilder(model::gradido::GradidoTransaction* baseTransaction);
	virtual ~CrossGroupTransactionBuilder();

	model::gradido::GradidoTransaction* createOutboundTransaction(const std::string& otherGroup);
	model::gradido::GradidoTransaction* createInboundTransaction(const std::string& otherGroup, MemoryBin* outboundMessageId);

protected:
	model::gradido::GradidoTransaction* mBaseTransaction;
};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_CROSS_GROUP_TRANSACTION_BUILDER_H