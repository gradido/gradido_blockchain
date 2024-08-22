#include "date/date.h"

#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/data/Protocol.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

namespace gradido {
	namespace blockchain {

		TransactionEntry::TransactionEntry(memory::ConstBlockPtr serializedTransaction)
			: mTransactionNr(0), mSerializedTransaction(serializedTransaction)
		{
			auto confirmedTransaction = getConfirmedTransaction();

			mTransactionNr = confirmedTransaction->mId;
			auto receivedDate = date::year_month_day{ date::floor<date::days>(confirmedTransaction->mConfirmedAt.getAsTimepoint()) };
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			mTransactionType = confirmedTransaction->mGradidoTransaction->getTransactionBody()->getTransactionType();
			mCoinCommunityId = getCoinCommunityId(*confirmedTransaction->mGradidoTransaction->getTransactionBody());
		}

		TransactionEntry::TransactionEntry(data::ConstConfirmedTransactionPtr confirmedTransaction)
			: mTransactionNr(confirmedTransaction->mId),
			mSerializedTransaction(interaction::serialize::Context(*confirmedTransaction).run()),
			mConfirmedTransaction(confirmedTransaction)
		{
			auto receivedDate = date::year_month_day{ date::floor<date::days>(confirmedTransaction->mConfirmedAt.getAsTimepoint()) };
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			auto body = confirmedTransaction->mGradidoTransaction->getTransactionBody();
			mTransactionType = body->getTransactionType();
			mCoinCommunityId = getCoinCommunityId(*body);
		}

		TransactionEntry::TransactionEntry(
			uint64_t transactionNr,
			date::month month,
			date::year year,
			data::TransactionType transactionType,
			std::string communityId
		) : 
			mTransactionNr(transactionNr),
			mMonth(month),
			mYear(year),
			mTransactionType(transactionType),
			mCoinCommunityId(communityId)
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
			auto transferAmount = body.getTransferAmount();
			if (transferAmount) {
				return transferAmount->communityId;
			}
			return "";
		}
	}
}
