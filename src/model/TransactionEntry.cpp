#include "gradido_blockchain/model/TransactionEntry.h"

#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"

namespace model {

	TransactionEntry::TransactionEntry(std::unique_ptr<std::string> _serializedTransaction)
		: mTransactionNr(0), mSerializedTransaction(std::move(_serializedTransaction))
	{
		auto transaction = std::make_unique<gradido::GradidoBlock>(mSerializedTransaction.get());

		mTransactionNr = transaction->getID();
		auto receivedDate = Poco::DateTime(transaction->getReceivedAsTimestamp());
		mMonth = receivedDate.month();
		mYear = receivedDate.year();
		mGroupId = transaction->getGradidoTransaction()->getTransactionBody()->getTransactionBase()->getGroupId();
	}

	TransactionEntry::TransactionEntry(gradido::GradidoBlock* transaction)
		: mTransactionNr(transaction->getID()), mSerializedTransaction(transaction->getSerialized())
	{
		auto received = Poco::DateTime(transaction->getReceivedAsTimestamp());
		mMonth = received.month();
		mYear = received.year();
		auto transactionBase = transaction->getGradidoTransaction()->getTransactionBody()->getTransactionBase();
		mGroupId = transactionBase->getGroupId();
	}

	TransactionEntry::TransactionEntry(uint64_t transactionNr, uint8_t month, uint16_t year, std::string groupId)
		: mTransactionNr(transactionNr), mMonth(month), mYear(year), mGroupId(groupId)
	{

	}
}
