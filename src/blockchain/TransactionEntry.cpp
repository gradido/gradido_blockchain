#include "date/date.h"

#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/memory/Block.h"

using std::optional, std::nullopt;
using memory::ConstBlockPtr;

namespace gradido {
	using data::ConstConfirmedTransactionPtr;
	namespace blockchain {

		TransactionEntry::TransactionEntry(ConstBlockPtr serializedTransaction, uint32_t blockchainCommunityIdIndex)
			: mTransactionNr(0), mSerializedTransaction(serializedTransaction), mBlockchainCommunityIdIndex(blockchainCommunityIdIndex)
		{
			auto confirmedTransaction = getConfirmedTransaction();

			mTransactionNr = confirmedTransaction->getId();
			auto receivedDate = date::year_month_day{ date::floor<date::days>(confirmedTransaction->getConfirmedAt().getAsTimepoint())};
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			mTransactionType = confirmedTransaction->getGradidoTransaction()->getTransactionBody()->getTransactionType();
			mCoinCommunityIdIndex = getCoinCommunityIdIndex(*confirmedTransaction->getGradidoTransaction()->getTransactionBody());
		}

		TransactionEntry::TransactionEntry(ConstConfirmedTransactionPtr confirmedTransaction, uint32_t blockchainCommunityIdIndex)
			: TransactionEntry(interaction::serialize::Context(*confirmedTransaction).run(blockchainCommunityIdIndex), confirmedTransaction, blockchainCommunityIdIndex)
		{
		}		

		TransactionEntry::TransactionEntry(
			ConstBlockPtr serializedTransaction, 
			ConstConfirmedTransactionPtr confirmedTransaction, 
			uint32_t blockchainCommunityIdIndex
		)
			: mTransactionNr(confirmedTransaction->getId()),
			mSerializedTransaction(serializedTransaction),
			mBlockchainCommunityIdIndex(blockchainCommunityIdIndex),
			mConfirmedTransaction(confirmedTransaction)
		{
			auto receivedDate = timepointAsYearMonthDay(confirmedTransaction->getConfirmedAt().getAsTimepoint());
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
			mTransactionType = body->getTransactionType();
			mCoinCommunityIdIndex = getCoinCommunityIdIndex(*body);
		}

		TransactionEntry::TransactionEntry(
			uint64_t transactionNr,
			date::month month,
			date::year year,
			data::TransactionType transactionType,
			optional<uint32_t> coinCommunityIdIndex,
			uint32_t blockchainCommunityIdIndex
		) : 
			mTransactionNr(transactionNr),
			mMonth(month),
			mYear(year),
			mTransactionType(transactionType),
			mCoinCommunityIdIndex(coinCommunityIdIndex),
			mBlockchainCommunityIdIndex(blockchainCommunityIdIndex)
		{

		}

		ConstConfirmedTransactionPtr TransactionEntry::getConfirmedTransaction() const
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

		optional<uint32_t> TransactionEntry::getCoinCommunityIdIndex(const data::TransactionBody& body) {
			if (body.hasTransferAmount()) {
				body.getTransferAmount().getCoinCommunityIdIndex();
			}
			return nullopt;
		}
	}
}
