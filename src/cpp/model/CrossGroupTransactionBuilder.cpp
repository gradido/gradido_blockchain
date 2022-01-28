#include "CrossGroupTransactionBuilder.h"

CrossGroupTransactionBuilder::CrossGroupTransactionBuilder(model::gradido::GradidoTransaction* baseTransaction)
	: mBaseTransaction(baseTransaction)
{

}

CrossGroupTransactionBuilder::~CrossGroupTransactionBuilder()
{
	if (mBaseTransaction) {
		delete mBaseTransaction;
		mBaseTransaction = nullptr;
	}

}

model::gradido::GradidoTransaction* CrossGroupTransactionBuilder::createOutboundTransaction(const std::string& otherGroup)
{

}
model::gradido::GradidoTransaction* CrossGroupTransactionBuilder::createInboundTransaction(const std::string& otherGroup, MemoryBin* outboundMessageId)
{

}
