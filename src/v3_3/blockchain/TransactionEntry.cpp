#include "date/date.h"

#include "gradido_blockchain/v3_3/blockchain/TransactionEntry.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"
#include "gradido_blockchain/v3_3/interaction/serialize/Context.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			TransactionEntry::TransactionEntry(memory::ConstBlockPtr serializedTransaction)
				: mTransactionNr(0), mSerializedTransaction(serializedTransaction)
			{
				auto confirmedTransaction = getConfirmedTransaction();

				mTransactionNr = confirmedTransaction->id;
				auto receivedDate = date::year_month_day{ date::floor<date::days>(confirmedTransaction->confirmedAt.getAsTimepoint()) };
				mMonth = static_cast<unsigned>(receivedDate.month());
				mYear = static_cast<int>(receivedDate.year());
				mTransactionType = confirmedTransaction->gradidoTransaction->getTransactionBody()->getTransactionType();
				mCommunityId = getCoinCommunityId(*confirmedTransaction->gradidoTransaction->getTransactionBody());
			}

			TransactionEntry::TransactionEntry(data::ConstConfirmedTransactionPtr confirmedTransaction)
				: mTransactionNr(confirmedTransaction->id),
				mSerializedTransaction(interaction::serialize::Context(*confirmedTransaction).run()),
				mConfirmedTransaction(confirmedTransaction)
			{
				auto receivedDate = date::year_month_day{ date::floor<date::days>(confirmedTransaction->confirmedAt.getAsTimepoint()) };
				mMonth = static_cast<unsigned>(receivedDate.month());
				mYear = static_cast<int>(receivedDate.year());
				auto body = confirmedTransaction->gradidoTransaction->getTransactionBody();
				mTransactionType = body->getTransactionType();
				mCommunityId = getCoinCommunityId(*body);
			}

			TransactionEntry::TransactionEntry(
				uint64_t transactionNr,
				uint8_t month,
				uint16_t year,
				data::TransactionType transactionType,
				std::string communityId
			) : 
				mTransactionNr(transactionNr),
				mMonth(month),
				mYear(year),
				mTransactionType(transactionType),
				mCommunityId(communityId)
			{

			}

			data::ConstConfirmedTransactionPtr TransactionEntry::getConfirmedTransaction() const
			{
				std::lock_guard _lock(mFastMutex);
				if (!mConfirmedTransaction) {
					interaction::deserialize::Context c(mSerializedTransaction, interaction::deserialize::Type::CONFIRMED_TRANSACTION);
					c.run();
					if (!c.isConfirmedTransaction()) {
						throw InvalidGradidoTransaction("v3_3::TransactionEntry::getConfirmedTransaction called, don't get expected ConfirmedTransaction", mSerializedTransaction);
					}
					mConfirmedTransaction = c.getConfirmedTransaction();
				}
				return mConfirmedTransaction;
			}

			std::string TransactionEntry::getCoinCommunityId(const data::TransactionBody& body)
			{
				if (body.isTransfer()) {
					return body.transfer->sender.communityId;
				}
				else if (body.isDeferredTransfer()) {
					return body.deferredTransfer->transfer.sender.communityId;
				}
				else if (body.isCreation()) {
					return body.creation->recipient.communityId;
				}
				return "";
			}
		}
	}
}
