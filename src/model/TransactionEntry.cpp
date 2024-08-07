#include "date/date.h"

#include "gradido_blockchain/model/TransactionEntry.h"

#include "gradido_blockchain/model/protobufWrapper/ConfirmedTransaction.h"

namespace model {

	TransactionEntry::TransactionEntry(std::unique_ptr<std::string> _serializedTransaction)
		: mTransactionNr(0), mSerializedTransaction(std::move(_serializedTransaction))
	{
		auto transaction = std::make_unique<gradido::ConfirmedTransaction>(mSerializedTransaction.get());

		mTransactionNr = transaction->getID();
		auto receivedDate = date::year_month_day{ date::floor<date::days>(transaction->getConfirmedAtAsTimepoint()) };
		mMonth = static_cast<unsigned>(receivedDate.month());
		mYear = static_cast<int>(receivedDate.year());
		mCommunityId = getCoinCommunityId(transaction->getGradidoTransaction()->getTransactionBody());
	}

	TransactionEntry::TransactionEntry(gradido::ConfirmedTransaction* transaction)
		: mTransactionNr(transaction->getID()), mSerializedTransaction(transaction->getSerialized())
	{
		auto receivedDate = date::year_month_day{ date::floor<date::days>(transaction->getConfirmedAtAsTimepoint()) };
		mMonth = static_cast<unsigned>(receivedDate.month());
		mYear = static_cast<int>(receivedDate.year());
		mCommunityId = getCoinCommunityId(transaction->getGradidoTransaction()->getTransactionBody());
	}

	TransactionEntry::TransactionEntry(uint64_t transactionNr, uint8_t month, uint16_t year, std::string communityId)
		: mTransactionNr(transactionNr), mMonth(month), mYear(year), mCommunityId(communityId)
	{

	}

	std::string TransactionEntry::getCoinCommunityId(const gradido::TransactionBody* body)
	{
		if (body->isTransfer() || body->isDeferredTransfer()) {
			return body->getTransferTransaction()->getCoinCommunityId();
		}
		else if (body->isCreation()) {
			return body->getCreationTransaction()->getCoinCommunityId();
		}
		return std::move("");
	}
}
