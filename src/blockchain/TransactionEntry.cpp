#include "date/date.h"

#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain_core/types/transaction.h"

#include <mutex>

using std::lock_guard;
using std::optional, std::nullopt;
using memory::ConstBlockPtr;

namespace gradido {
	using data::ConstConfirmedTransactionPtr;
	using data::ConstTransactionBodyPtr;

	namespace blockchain {

		TransactionEntry::TransactionEntry(ConstBlockPtr serializedTransaction, uint32_t blockchainCommunityIdIndex)
			: mSerializedTransaction(serializedTransaction), mBlockchainCommunityIdIndex(blockchainCommunityIdIndex)
		{
			mConfirmedTransaction = getConfirmedTransaction();
			mTransactionNr = mConfirmedTransaction->getId();
			auto receivedDate = timepointAsYearMonthDay(mConfirmedTransaction->getConfirmedAt().getAsTimepoint());
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			auto body = mConfirmedTransaction->getGradidoTransaction()->getTransactionBody();
			mTransactionType = body->getTransactionType();
			mCoinCommunityIdIndex = getCoinCommunityIdIndex(*body);
		}

		TransactionEntry::TransactionEntry(ConstConfirmedTransactionPtr confirmedTransaction, uint32_t blockchainCommunityIdIndex)
			: mTransactionNr(confirmedTransaction->getId()), 
			mConfirmedTransaction(confirmedTransaction), 
			mBlockchainCommunityIdIndex(blockchainCommunityIdIndex)
		{
			auto receivedDate = timepointAsYearMonthDay(confirmedTransaction->getConfirmedAt().getAsTimepoint());
			mMonth = receivedDate.month();
			mYear = receivedDate.year();
			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
			mTransactionType = body->getTransactionType();
			mCoinCommunityIdIndex = getCoinCommunityIdIndex(*body);
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
			grdt_transaction transactionType,
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

		ConstBlockPtr TransactionEntry::getSerializedTransaction() const
		{
			lock_guard _lock(mFastMutex);
			if (!mSerializedTransaction && !mConfirmedTransaction) return nullptr;
			if (!mSerializedTransaction && mConfirmedTransaction) {
				interaction::serialize::Context c(*mConfirmedTransaction);
				mSerializedTransaction = c.run();
				if (!mSerializedTransaction) {
					throw GradidoNodeInvalidDataException("TransactionEntry::getSerializedTransaction called, serialize failed!");
				}
			}
			return mSerializedTransaction;
		}

		ConstConfirmedTransactionPtr TransactionEntry::getConfirmedTransaction() const
		{
			lock_guard _lock(mFastMutex);
			if (!mSerializedTransaction && !mConfirmedTransaction) return nullptr;
			if (!mConfirmedTransaction && mSerializedTransaction) {
				interaction::deserialize::Context c(mSerializedTransaction, interaction::deserialize::Type::CONFIRMED_TRANSACTION);
				c.run(mBlockchainCommunityIdIndex);
				if (!c.isConfirmedTransaction()) {
					throw InvalidGradidoTransaction("TransactionEntry::getConfirmedTransaction called, don't get expected ConfirmedTransaction", mSerializedTransaction);
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

		ConstTransactionBodyPtr TransactionEntry::getTransactionBody() const 
		{ 
			return getConfirmedTransaction()->getGradidoTransaction()->getTransactionBody(); 
		}
	}
}
