#include "TransactionFactory.h"
#include "lib/DataTypeConverter.h"

using namespace model::gradido;

GradidoTransaction* TransactionFactory::createDeferredTransfer(GradidoTransaction* transfer, Poco::Timestamp timeout)
{
	transfer->getMutableTransactionBody()->upgradeToDeferredTransaction(timeout);
	return transfer;	
}

GradidoTransaction* TransactionFactory::createGlobalGroupAdd(const std::string& groupName, const std::string& groupAlias, uint32_t nativeCoinColor)
{
	return new GradidoTransaction(TransactionBody::createGlobalGroupAdd(groupName, groupAlias, nativeCoinColor));
}

GradidoTransaction* TransactionFactory::createGroupFriendsUpdate(bool colorFusion)
{
	return new GradidoTransaction(TransactionBody::createGroupFriendsUpdate(colorFusion));
}

model::gradido::GradidoTransaction* TransactionFactory::createRegisterAddress(
	const MemoryBin* userPubkey,
	proto::gradido::RegisterAddress_AddressType type,
	const MemoryBin* nameHash,
	const MemoryBin* subaccountPubkey
)
{
	return new GradidoTransaction(TransactionBody::createRegisterAddress(userPubkey, type, nameHash, subaccountPubkey));
}

model::gradido::GradidoTransaction* TransactionFactory::createTransactionCreation(
	const MemoryBin* recipientPubkey,
	int64_t amountGddCent,
	uint32_t coinColor,
	Poco::DateTime targetDate
)
{
	auto transferAmount = createTransferAmount(recipientPubkey, amountGddCent, coinColor);
	return new GradidoTransaction(TransactionBody::createTransactionCreation(std::move(transferAmount), targetDate));
}

model::gradido::GradidoTransaction* TransactionFactory::createTransactionTransfer(
	const MemoryBin* senderPubkey,
	int64_t amountGddCent,
	uint32_t coinColor,
	const MemoryBin* recipientPubkey
)
{
	assert(recipientPubkey);
	auto transferAmount = createTransferAmount(recipientPubkey, amountGddCent, coinColor);
	return new GradidoTransaction(TransactionBody::createTransactionTransfer(std::move(transferAmount), recipientPubkey));
}

std::unique_ptr<proto::gradido::TransferAmount> TransactionFactory::createTransferAmount(
	const MemoryBin* recipientPubkey,
	int64_t amountGddCent,
	uint32_t coinColor
)
{
	assert(recipientPubkey);
	auto transferAmount = std::unique_ptr<proto::gradido::TransferAmount>(new proto::gradido::TransferAmount);
	transferAmount->set_pubkey(recipientPubkey->copyAsString());
	transferAmount->set_amount(amountGddCent);
	transferAmount->set_coin_color(coinColor);
	return transferAmount;
}