#include "gradido_blockchain/v3_3/blockchain/InMemory.h"
#include "gradido_blockchain/v3_3/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/v3_3/interaction/validate/Context.h"
#include "gradido_blockchain/v3_3/interaction/validate/Type.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/v3_3/const.h"
#include "gradido_blockchain/v3_3/blockchain/FilterBuilder.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			InMemory::InMemory(std::string_view communityId)
				: Abstract(communityId), mSortedDirty(false), mExitCalled(false)
			{

			}

			InMemory::~InMemory()
			{
			}

			void InMemory::clear()
			{
				std::lock_guard _lock(mWorkMutex);
				mTransactionsByPubkey.clear();
				mTransactionsByConfirmedAt.clear();
				mSortedDirty = false;
				mSortedTransactions.clear();
			}

			void InMemory::exit()
			{
				std::lock_guard _lock(mWorkMutex);
				mExitCalled = true;
			}

			bool InMemory::addGradidoTransaction(data::ConstGradidoTransactionPtr gradidoTransaction)
			{
				std::lock_guard _lock(mWorkMutex);
				if (mExitCalled) return;
				if (isTransactionExist(gradidoTransaction)) {
					return false;
				}
				uint64_t id = 1;
				auto lastTransaction = findOne(Filter::LAST_TRANSACTION);
				if (lastTransaction) {
					id = lastTransaction->getTransactionNr() + 1;
				}
				
				auto serializedTransaction = gradidoTransaction->getSerializedTransaction();
				auto body = gradidoTransaction->getTransactionBody();
				// fake message id simply with taking hash from serialized transaction,
				// iota message id will normally calculated with same algorithmus but with additional data 
				auto messageId = std::make_shared<memory::Block>(serializedTransaction->calculateHash());			

				interaction::calculateAccountBalance::Context finalBalanceCalculate(*this);
				auto finalBalance = finalBalanceCalculate.run(gradidoTransaction, body->createdAt.getAsTimepoint(), id);

				auto confirmedTransaction = std::make_shared<data::ConfirmedTransaction>(
					id,
					gradidoTransaction,
					body->createdAt,
					GRADIDO_CONFIRMED_TRANSACTION_V3_3_VERSION_STRING,
					nullptr,
					messageId,
					finalBalance
				);
				data::ConstConfirmedTransactionPtr lastConfirmedTransaction;
				if (lastTransaction) {
					lastConfirmedTransaction = lastTransaction->getConfirmedTransaction();
					if (body->createdAt < lastConfirmedTransaction->confirmedAt) {
						throw BlockchainOrderException("previous transaction is younger");
					}
				}
				confirmedTransaction->runningHash = std::make_shared<memory::Block>(confirmedTransaction->calculateRunningHash(lastConfirmedTransaction));
				// important! validation
				interaction::validate::Context validate(*confirmedTransaction);
				interaction::validate::Type level =
					interaction::validate::Type::SINGLE |
					interaction::validate::Type::PREVIOUS |
					interaction::validate::Type::MONTH_RANGE |
					interaction::validate::Type::ACCOUNT
				;
				if (body->type != data::CrossGroupType::LOCAL) {
					level = level | interaction::validate::Type::PAIRED;
				}
				// throw if some error occure
				validate.run(level, getCommunityId(), InMemoryProvider::getInstance());

				auto transactionEntry = std::make_shared<TransactionEntry>(confirmedTransaction);
				pushTransactionEntry(transactionEntry);
				mTransactionFingerprintTransactionEntry.insert({ confirmedTransaction->gradidoTransaction->getFingerprint(), transactionEntry });
			}

			void InMemory::pushTransactionEntry(std::shared_ptr<TransactionEntry> transactionEntry)
			{
				std::lock_guard _lock(mWorkMutex);
				mSortedDirty = true;
				auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
				auto involvedAddresses = confirmedTransaction->gradidoTransaction->getInvolvedAddresses();

				mTransactionsByConfirmedAt.insert({ confirmedTransaction->confirmedAt, transactionEntry });
				for (auto involvedAddress : involvedAddresses) {
					mTransactionsByPubkey.insert({ involvedAddress, transactionEntry });
				}
				mMessageIdTransactionNrs.insert({ 
					iota::MessageId::fromMemoryBlock(*confirmedTransaction->messageId),
					confirmedTransaction->id
				});
				mTransactionsByNr.insert({
					confirmedTransaction->id,
					transactionEntry
				});
				auto body = confirmedTransaction->gradidoTransaction->getTransactionBody();
				// add deferred transfer to special deferred transfer map
				if (body->isDeferredTransfer()) {
					mTimeoutDeferredRedeemedTransferPairs.insert({ body->deferredTransfer->timeout.getAsTimepoint(), {transactionEntry, nullptr} });
				}
				// find out if transaction redeem a deferred transfer
				if (body->isTransfer()) {
					FilterBuilder filterBuilder;
					filterBuilder
						.setInvolvedPublicKey(body->transfer->sender.pubkey)
						.setMaxTransactionNr(confirmedTransaction->id - 1)
						.setSearchDirection(SearchDirection::DESC)
						.setPagination(Pagination(1))
					;
					auto lastFromSameSender = findOne(filterBuilder.build());
					if (lastFromSameSender) {
						auto lastFromSameSenderBody = lastFromSameSender->getConfirmedTransaction()->gradidoTransaction->getTransactionBody();
						if (lastFromSameSenderBody->isDeferredTransfer() &&
							lastFromSameSenderBody->deferredTransfer->transfer.recipient->isTheSame(body->transfer->sender.pubkey)) {
							// seems we found a matching deferred transfer transaction
							auto byTimeoutDeferredRedeemedPairsIt = mTimeoutDeferredRedeemedTransferPairs.equal_range(lastFromSameSenderBody->deferredTransfer->timeout.getAsTimepoint());
							for (auto it = byTimeoutDeferredRedeemedPairsIt.first; it != byTimeoutDeferredRedeemedPairsIt.second; ++it) {
								if (lastFromSameSender->getSerializedTransaction()->isTheSame(it->second.first->getSerializedTransaction())) {
									it->second.second = transactionEntry;
									break;
								}
							}
						}
					}
				}
			}

			void InMemory::removeTransactionEntry(std::shared_ptr<TransactionEntry> transactionEntry)
			{
				std::lock_guard _lock(mWorkMutex);
				mSortedDirty = true;
				auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
				auto byReceivedIt = mTransactionsByConfirmedAt.equal_range(confirmedTransaction->confirmedAt);
				auto serializedTransaction = transactionEntry->getSerializedTransaction();
				for (auto it = byReceivedIt.first; it != byReceivedIt.second; ++it)
				{
					if (serializedTransaction->isTheSame(it->second->getSerializedTransaction()))
					{
						it = mTransactionsByConfirmedAt.erase(it);
						if (it == byReceivedIt.second) {
							break;
						}
					}
				}
				auto involvedAddresses = confirmedTransaction->gradidoTransaction->getInvolvedAddresses();
				for (auto involvedAddress : involvedAddresses)
				{
					auto byPublicKeyIt = mTransactionsByPubkey.equal_range(involvedAddress);
					for (auto it = byPublicKeyIt.first; it != byPublicKeyIt.second; ++it)
					{
						it = mTransactionsByPubkey.erase(it);
						if (it == byPublicKeyIt.second) {
							break;
						}
					}
				}
				mMessageIdTransactionNrs.erase(iota::MessageId::fromMemoryBlock(*confirmedTransaction->messageId));
				mTransactionsByNr.erase(confirmedTransaction->id);

				auto body = confirmedTransaction->gradidoTransaction->getTransactionBody();
				// remove deferred transfer from special deferred transfer map
				if (body->isDeferredTransfer()) {
					auto byTimeoutDeferredRedeemedPairsIt = mTimeoutDeferredRedeemedTransferPairs.equal_range(body->deferredTransfer->timeout.getAsTimepoint());
					for (auto it = byTimeoutDeferredRedeemedPairsIt.first; it != byTimeoutDeferredRedeemedPairsIt.second; ++it) {
						if (serializedTransaction->isTheSame(it->second.first->getSerializedTransaction())) {
							mTimeoutDeferredRedeemedTransferPairs.erase(it);
							break;
						}
					}
				}
			}

			//! find all transfers which redeem a deferred transfer in date range
				//! \param senderPublicKey sender public key of sending account of redeem transaction
				//! \return list with transaction pairs, first is deferred transfer, second is redeeming transfer
			std::list<DeferredRedeemedTransferPair> InMemory::findRedeemedDeferredTransfersInRange(
				memory::ConstBlockPtr senderPublicKey,
				Timepoint start,
				Timepoint end,
				uint64_t maxTransactionNr
			) const
			{

			}

			std::shared_ptr<TransactionEntry> InMemory::getTransactionForId(uint64_t transactionId) const
			{
				std::lock_guard _lock(mWorkMutex);
				auto it = mTransactionsByNr.find(transactionId);
				if (it != mTransactionsByNr.end()) {
					return it->second;
				}
				return nullptr;
			}

			std::shared_ptr<TransactionEntry> InMemory::findByMessageId(
				memory::ConstBlockPtr messageId,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const
			{
				std::lock_guard _lock(mWorkMutex);
				auto it = mMessageIdTransactionNrs.find(iota::MessageId::fromMemoryBlock(*messageId));
				if (it != mMessageIdTransactionNrs.end()) {
					return getTransactionForId(it->second);
				}
				return nullptr;
			}

			bool InMemory::isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const
			{
				std::lock_guard _lock(mWorkMutex);
				auto signature = gradidoTransaction->getFingerprint();
				auto range = mTransactionFingerprintTransactionEntry.equal_range(signature);
				for (auto it = range.first; it != range.second; ++it) {
					if (it->second->getConfirmedTransaction()->gradidoTransaction->getFingerprint()->isTheSame(signature)) {
						return true;
					}
				}	
				return false;
			}
		}
	}
}