#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H

#include "protobufWrapper/ConfirmedTransaction.h"

#include <chrono>

class GRADIDOBLOCKCHAIN_EXPORT TransactionFactory
{
public:
	static std::unique_ptr<model::gradido::GradidoTransaction> createDeferredTransfer(
		std::unique_ptr<model::gradido::GradidoTransaction> transfer, 
		Timepoint timeout
	);
	static std::unique_ptr<model::gradido::GradidoTransaction> createGroupFriendsUpdate(bool colorFusion);
	static std::unique_ptr<model::gradido::GradidoTransaction> createRegisterAddress(
		const MemoryBin* userPubkey,
		proto::gradido::RegisterAddress_AddressType type,
		const MemoryBin* nameHash = nullptr,
		const MemoryBin* accountPubkey = nullptr
	);
	static std::unique_ptr<model::gradido::GradidoTransaction> createTransactionCreation(
		const MemoryBin* recipientPubkey,
		const std::string& amountGddCent,
		Timepoint targetDate
	);
	static std::unique_ptr<model::gradido::GradidoTransaction> createTransactionTransfer(
		const MemoryBin* senderPubkey,
		const std::string& amountGddCent,
		std::string communityId,
		const MemoryBin* recipientPubkey
	);
	static std::unique_ptr<model::gradido::GradidoTransaction> createCommunityRoot(
		const MemoryBin* pubkey,
		const MemoryBin* gmwPubkey,
		const MemoryBin* aufPubkey
	);
	static std::shared_ptr<model::gradido::ConfirmedTransaction> createConfirmedTransaction(
		std::unique_ptr<model::gradido::GradidoTransaction> transaction,
		uint64_t id,
		int64_t received,
		const MemoryBin* messageId
	);	

protected:
	static std::unique_ptr<proto::gradido::TransferAmount> createTransferAmount(
		const MemoryBin* recipientPubkey,
		const std::string& amountGddCent,
		std::string groupId
	);
};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H