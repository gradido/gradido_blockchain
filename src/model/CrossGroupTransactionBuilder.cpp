#include "gradido_blockchain/model/CrossGroupTransactionBuilder.h"

CrossGroupTransactionBuilder::CrossGroupTransactionBuilder(std::unique_ptr<model::gradido::GradidoTransaction> baseTransaction)
	: mBaseTransaction(std::move(baseTransaction))
{

}

CrossGroupTransactionBuilder::~CrossGroupTransactionBuilder()
{

}

std::unique_ptr<model::gradido::GradidoTransaction> CrossGroupTransactionBuilder::createOutboundTransaction(const std::string& otherGroup)
{
	auto serializedTransaction = mBaseTransaction->getSerialized();
	std::unique_ptr<model::gradido::GradidoTransaction> outbound(new model::gradido::GradidoTransaction(*serializedTransaction));
	outbound->getMutableTransactionBody()->updateToOutbound(otherGroup);
	return outbound;
}
std::unique_ptr<model::gradido::GradidoTransaction> CrossGroupTransactionBuilder::createInboundTransaction(const std::string& otherGroup, MemoryBin* outboundMessageId)
{
	assert(outboundMessageId);
	mBaseTransaction->setParentMessageId(outboundMessageId);
	return std::move(mBaseTransaction);
}

std::unique_ptr<model::gradido::GradidoTransaction> CrossGroupTransactionBuilder::createInboundTransaction(const std::string& otherGroup)
{
	mBaseTransaction->getMutableTransactionBody()->updateToInbound(otherGroup);
	return std::move(mBaseTransaction);
}