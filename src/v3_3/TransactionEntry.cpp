#include "date/date.h"

#include "gradido_blockchain/v3_3/TransactionEntry.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"
#include "gradido_blockchain/v3_3/interaction/serialize/Context.h"

namespace gradido {
	namespace v3_3 {

		TransactionEntry::TransactionEntry(memory::ConstBlockPtr serializedTransaction)
			: mTransactionNr(0), mSerializedTransaction(serializedTransaction)
		{
			interaction::deserialize::Context c(serializedTransaction, interaction::deserialize::Type::CONFIRMED_TRANSACTION);
			c.run();
			if(!c.isConfirmedTransaction()) {
				throw InvalidGradidoTransaction("TransactionEntry constructor called, don't get expected ConfirmedTransaction", serializedTransaction);
			}
			auto transaction = c.getConfirmedTransaction();

			mTransactionNr = transaction->id;
			auto receivedDate = date::year_month_day{ date::floor<date::days>(transaction->confirmedAt) };
			mMonth = static_cast<unsigned>(receivedDate.month());
			mYear = static_cast<int>(receivedDate.year());
			mCommunityId = getCoinCommunityId(transaction->gradidoTransaction->bodyBytes);
		}

		TransactionEntry::TransactionEntry(const data::ConfirmedTransaction& transaction)
			: mTransactionNr(transaction.id), mSerializedTransaction(interaction::serialize::Context(transaction).run())
		{
			auto receivedDate = date::year_month_day{ date::floor<date::days>(transaction.confirmedAt) };
			mMonth = static_cast<unsigned>(receivedDate.month());
			mYear = static_cast<int>(receivedDate.year());
			mCommunityId = getCoinCommunityId(transaction.gradidoTransaction->bodyBytes);
		}

		TransactionEntry::TransactionEntry(uint64_t transactionNr, uint8_t month, uint16_t year, std::string communityId)
			: mTransactionNr(transactionNr), mMonth(month), mYear(year), mCommunityId(communityId)
		{

		}

		std::string TransactionEntry::getCoinCommunityId(memory::ConstBlockPtr bodyBytes)
		{
			interaction::deserialize::Context c(bodyBytes, interaction::deserialize::Type::TRANSACTION_BODY);
			c.run();
			if(!c.isTransactionBody()) {
				throw InvalidGradidoTransaction("TransactionEntry::getCoinCommunityId called, don't get expected raw TransactionBody", bodyBytes);
			}
			auto body = c.getTransactionBody();
			if (body->isTransfer()) {
				return body->transfer->sender.communityId;
			} else if(body->isDeferredTransfer()) {
				return body->deferredTransfer->transfer.sender.communityId;
			}
			else if (body->isCreation()) {
				return body->creation->recipient.communityId;
			}
			return std::move("");
		}
	}
}
