#include "gradido_blockchain/v3_3/blockchain/InMemory.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			InMemory::InMemory(std::string_view communityId)
				: mCommunityId(communityId), mSortedDirty(false)
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

			void InMemory::pushGradidoTransaction(std::shared_ptr<TransactionEntry> transactionEntry)
			{
				std::lock_guard _lock(mWorkMutex);

				mSortedDirty = true;
				auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
				auto involvedAddresses = confirmedTransaction->gradidoTransaction->getInvolvedAddresses();

				mTransactionsByConfirmedAt.insert({ confirmedTransaction->confirmedAt, transactionEntry });
				for (auto involvedAddress : involvedAddresses) {
					mTransactionsByPubkey.insert({ involvedAddress, transactionEntry });
				}
			}

			void InMemory::removeGradidoTransaction(std::shared_ptr<TransactionEntry> transactionEntry)
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
			}

			InMemory::UserBalance InMemory::calculateUserBalanceUntil(memory::ConstBlockPtr userPubkey, Timepoint date) {
				auto transactions = getSortedTransactionsForUser(groupAlias, pubkeyHex);
				auto mm = MemoryManager::getInstance();
				DecayDecimal balance;
				DecayDecimal amount;
				auto pubkeyBinString = DataTypeConverter::hexToBinString(pubkeyHex.substr(0, 64));

				Timepoint now = system_clock::now();
				Timepoint lastBalanceDate(now);
				for (auto it = transactions.begin(); it != transactions.end(); it++) {
					auto transactionBody = (*it)->getTransactionBody();
					Timepoint localDate(std::chrono::seconds(transactionBody->getCreatedAtSeconds()));
					if (localDate > date) break;
					if (localDate > lastBalanceDate && lastBalanceDate != now) {
						balance.applyDecay(lastBalanceDate, localDate);
					}
					std::string amountString;
					bool subtract = false;
					if (transactionBody->isCreation()) {
						auto creation = transactionBody->getCreationTransaction();
						amountString = creation->getAmount();
					}
					else if (transactionBody->isTransfer()) {
						auto transfer = transactionBody->getTransferTransaction();
						amountString = transfer->getAmount();
						if (transfer->getRecipientPublicKeyString() == *pubkeyBinString) {

						}
						else if (transfer->getSenderPublicKeyString() == *pubkeyBinString) {
							subtract = true;
						}
						else {
							assert(true || "transaction don't belong to us");
						}
					}
					else {
						continue;
					}
					amount = amountString;
					if (!subtract) {
						balance += amount;
					}
					else {
						balance -= amount;
					}
					lastBalanceDate = localDate;
				}
				if (date > lastBalanceDate && lastBalanceDate != now) {
					balance.applyDecay(lastBalanceDate, date);
				}
				std::string balanceString;
				model::gradido::TransactionBase::amountToString(&balanceString, balance);
				return UserBalance(pubkeyHex, balanceString, date);
			}
		}
	}
}