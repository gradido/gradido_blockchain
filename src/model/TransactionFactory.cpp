#include "gradido_blockchain/model/TransactionFactory.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

using namespace model::gradido;

std::unique_ptr<model::gradido::GradidoTransaction> TransactionFactory::createDeferredTransfer(std::unique_ptr<model::gradido::GradidoTransaction> transfer, Poco::Timestamp timeout)
{
	transfer->getMutableTransactionBody()->upgradeToDeferredTransaction(timeout);
	return transfer;	
}

std::unique_ptr<model::gradido::GradidoTransaction> TransactionFactory::createGroupFriendsUpdate(bool colorFusion)
{
	return std::unique_ptr<model::gradido::GradidoTransaction>(new GradidoTransaction(TransactionBody::createGroupFriendsUpdate(colorFusion)));
}

std::unique_ptr<model::gradido::GradidoTransaction> TransactionFactory::createRegisterAddress(
	const MemoryBin* userPubkey,
	proto::gradido::RegisterAddress_AddressType type,
	const MemoryBin* nameHash,
	const MemoryBin* subaccountPubkey
)
{
	return std::unique_ptr<model::gradido::GradidoTransaction>(new GradidoTransaction(TransactionBody::createRegisterAddress(userPubkey, type, nameHash, subaccountPubkey)));
}

std::unique_ptr<model::gradido::GradidoTransaction> TransactionFactory::createTransactionCreation(
	const MemoryBin* recipientPubkey,
	const std::string& amountGddCent,
	std::string groupId,
	Poco::DateTime targetDate
)
{
	auto transferAmount = createTransferAmount(recipientPubkey, amountGddCent, groupId);
	return std::unique_ptr<model::gradido::GradidoTransaction>(new GradidoTransaction(TransactionBody::createTransactionCreation(std::move(transferAmount), targetDate)));
}

std::unique_ptr<model::gradido::GradidoTransaction> TransactionFactory::createTransactionTransfer(
	const MemoryBin* senderPubkey,
	const std::string& amountGdd,
	std::string groupId,
	const MemoryBin* recipientPubkey
)
{
	assert(recipientPubkey);
	auto transferAmount = createTransferAmount(senderPubkey, amountGdd, groupId);
	return std::unique_ptr<model::gradido::GradidoTransaction>(new GradidoTransaction(TransactionBody::createTransactionTransfer(std::move(transferAmount), recipientPubkey)));
}

Poco::SharedPtr<model::gradido::GradidoBlock> TransactionFactory::createGradidoBlock(
	std::unique_ptr<model::gradido::GradidoTransaction> transaction,
	uint64_t id,
	int64_t received,
	const MemoryBin* messageId
)
{
	return model::gradido::GradidoBlock::create(std::move(transaction), id, received, messageId);
}

std::unique_ptr<proto::gradido::TransferAmount> TransactionFactory::createTransferAmount(
	const MemoryBin* recipientPubkey,
	const std::string& amountGddCent,
	std::string groupId
)
{
	assert(recipientPubkey);
	auto transferAmount = std::unique_ptr<proto::gradido::TransferAmount>(new proto::gradido::TransferAmount);
	transferAmount->set_allocated_pubkey(recipientPubkey->copyAsString().release());
	transferAmount->set_amount(amountGddCent);
	transferAmount->set_group_id(groupId);
	return transferAmount;
}

