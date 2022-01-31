#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H

#include "protobufWrapper/GradidoTransaction.h"
#include "Poco/Timestamp.h"

class TransactionFactory
{
public:
	static model::gradido::GradidoTransaction* createDeferredTransfer(model::gradido::GradidoTransaction* transfer, Poco::Timestamp timeout);
	static model::gradido::GradidoTransaction* createGlobalGroupAdd(const std::string& groupName, const std::string& groupAlias, uint32_t nativeCoinColor);
	static model::gradido::GradidoTransaction* createGroupFriendsUpdate(bool colorFusion);
	static model::gradido::GradidoTransaction* createRegisterAddress(
		const MemoryBin* userPubkey,
		proto::gradido::RegisterAddress_AddressType type,
		const MemoryBin* nameHash,
		const MemoryBin* subaccountPubkey
	);
	static model::gradido::GradidoTransaction* createTransactionCreation(
		const MemoryBin* recipientPubkey,
		int64_t amountGddCent,
		uint32_t coinColor,
		Poco::DateTime targetDate
	);
	static model::gradido::GradidoTransaction* createTransactionTransfer(
		const MemoryBin* senderPubkey,
		int64_t amountGddCent,
		uint32_t coinColor,
		const MemoryBin* recipientPubkey
	);

protected:
	static std::unique_ptr<proto::gradido::TransferAmount> createTransferAmount(
		const MemoryBin* recipientPubkey,
		int64_t amountGddCent,
		uint32_t coinColor
	);
};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H