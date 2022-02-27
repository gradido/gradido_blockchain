#include "gradido_blockchain/model/TransactionEntry.h"

#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"

namespace model {

	TransactionEntry::TransactionEntry(std::unique_ptr<std::string> _serializedTransaction)
		: mTransactionNr(0), mSerializedTransaction(std::move(_serializedTransaction))
	{
		auto transaction = std::make_unique<gradido::GradidoBlock>(mSerializedTransaction.get());

		mTransactionNr = transaction->getID();
		mMonth = transaction->getReceived().month();
		mYear = transaction->getReceived().year();
		mCoinColor = transaction->getGradidoTransaction()->getTransactionBody()->getTransactionBase()->getCoinColor();
	}

	TransactionEntry::TransactionEntry(gradido::GradidoBlock* transaction)
		: mTransactionNr(transaction->getID()), mSerializedTransaction(transaction->getSerialized())
	{
		auto received = transaction->getReceived();
		mMonth = received.month();
		mYear = received.year();
		auto transactionBase = transaction->getGradidoTransaction()->getTransactionBody()->getTransactionBase();
		mCoinColor = transactionBase->getCoinColor();
	}

	TransactionEntry::TransactionEntry(uint64_t transactionNr, uint8_t month, uint16_t year, uint32_t coinColor)
		: mTransactionNr(transactionNr), mMonth(month), mYear(year), mCoinColor(coinColor)
	{

	}
}
