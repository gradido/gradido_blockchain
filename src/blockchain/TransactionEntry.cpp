#include "date/date.h"

#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

namespace gradido {
	namespace blockchain {

		TransactionEntry::TransactionEntry(memory::ConstBlockPtr serializedTransaction)
			: mTransactionNr(0), mSerializedTransaction(serializedTransaction)
		{
			auto confirmedTransaction = getConfirmedTransaction();

			mTransactionNr = confirmedTransaction->getId();
			auto receivedDate = date::year_month_day{ date::floor<date::days>(confirmedTransaction->getConfirmedAt().getAsTimepoint())};
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			mTransactionType = confirmedTransaction->getGradidoTransaction()->getTransactionBody()->getTransactionType();
			mCoinCommunityId = getCoinCommunityId(*confirmedTransaction->getGradidoTransaction()->getTransactionBody());
		}

		TransactionEntry::TransactionEntry(data::ConstConfirmedTransactionPtr confirmedTransaction)
			: mTransactionNr(confirmedTransaction->getId()),
			mSerializedTransaction(interaction::serialize::Context(*confirmedTransaction).run()),
			mConfirmedTransaction(confirmedTransaction)
		{
			auto receivedDate = timepointAsYearMonthDay(confirmedTransaction->getConfirmedAt().getAsTimepoint());
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
			mTransactionType = body->getTransactionType();
			mCoinCommunityId = getCoinCommunityId(*body);
		}

		TransactionEntry::TransactionEntry(std::shared_ptr<data::EventTriggeredTransaction> eventTriggeredTransaction)
			: mTransactionNr(eventTriggeredTransaction->getId()), mTransactionType(data::TransactionType::NONE)
		{
			auto targetDate = timepointAsYearMonthDay(eventTriggeredTransaction->getTargetDate().getAsTimepoint());
			mMonth = targetDate.month();
			mYear = targetDate.year();
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
				return transferAmount->getCommunityId();
			}
			return "";
		}
	}
}
