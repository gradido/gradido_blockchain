#include "TransactionFactory.h"

model::gradido::GradidoTransaction* TransactionFactory::createDeferredTransfer(model::gradido::GradidoTransaction* transfer, Poco::Timestamp timeout)
{

}

model::gradido::GradidoTransaction* TransactionFactory::createGlobalGroupAdd(const std::string& groupName, const std::string& groupAlias, uint32_t nativeCoinColor)
{

}

model::gradido::GradidoTransaction* TransactionFactory::createGroupFriendsUpdate(bool colorFusion)
{

}

model::gradido::GradidoTransaction* TransactionFactory::createRegisterAddress(
	MemoryBin* userPubkey,
	proto::gradido::RegisterAddress_AddressType type,
	MemoryBin* nameHash,
	MemoryBin* subaccountPubkey
)
{

}

model::gradido::GradidoTransaction* TransactionFactory::createTransactionCreation(
	MemoryBin* recipientPubkey,
	int64_t amountGddCent,
	uint32_t coinColor,
	Poco::DateTime targetDate
)
{

}

model::gradido::GradidoTransaction* TransactionFactory::createTransactionTransfer(
	MemoryBin* senderPubkey,
	int64_t amountGddCent,
	uint32_t coinColor,
	MemoryBin* recipientPubkey
)
{

}