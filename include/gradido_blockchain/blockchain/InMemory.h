#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H

#include "Abstract.h"
#include "FilterResult.h"
#include "../data/Protocol.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"

#include <vector>
#include <unordered_map>
#include <shared_mutex>		

namespace gradido {
	namespace blockchain {
		/*
		* @author einhornimmond
		* @date 16.06.2024
		* @brief  A easy system for holding transactions and check them
		*
		* Not optimized for unlimited transactions, mostly used for testing, debugging and performance tests
		*/
		class GRADIDOBLOCKCHAIN_EXPORT InMemory : public Abstract
		{
		public:
			InMemory(std::string_view communityId);
			~InMemory();

			// remove all transactions and start over
			void clear();
			void exit();

			//! validate and generate confirmed transaction
			//! throw if gradido transaction isn't valid
			//! \return false if transaction already exist
			bool addGradidoTransaction(data::ConstGradidoTransactionPtr gradidoTransaction);

			struct UserBalance
			{
				UserBalance(memory::ConstBlockPtr _userPubkey, GradidoUnit _balance, Timepoint _balanceDate)
					: userPubkey(_userPubkey), balance(_balance), balanceDate(_balanceDate) {}
				memory::ConstBlockPtr userPubkey;
				GradidoUnit balance;
				Timepoint balanceDate;
			};

			//std::string getUserTransactionsDebugString(const std::string& groupAlias, const std::string& pubkeyHex);

			// get all transactions sorted by id
			const TransactionEntries& getSortedTransactions();

			// from Abstract blockchain
			TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const;

			//! find all deferred transfers which have the timeout in date range between start and end, have senderPublicKey and are not redeemed,
			//! therefore boocked back to sender
			TransactionEntries findTimeoutedDeferredTransfersInRange(
				memory::ConstBlockPtr senderPublicKey,
				TimepointInterval timepointInterval,
				uint64_t maxTransactionNr
			) const;

			//! find all transfers which redeem a deferred transfer in date range
			//! \param senderPublicKey sender public key of sending account of deferred transaction
			//! \return list with transaction pairs, first is deferred transfer, second is redeeming transfer
			std::list<DeferredRedeemedTransferPair> findRedeemedDeferredTransfersInRange(
				memory::ConstBlockPtr senderPublicKey,
				TimepointInterval timepointInterval,
				uint64_t maxTransactionNr
			) const;

			std::shared_ptr<TransactionEntry> getTransactionForId(uint64_t transactionId) const;

			// this implementation use a map for direct search and don't use filter at all
			std::shared_ptr<TransactionEntry> findByMessageId(
				memory::ConstBlockPtr messageId,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const;

		protected:
			// if called, mWorkMutex should be locked exclusive
			void pushTransactionEntry(std::shared_ptr<TransactionEntry> transactionEntry);
			void removeTransactionEntry(std::shared_ptr<TransactionEntry> transactionEntry);

			FilterCriteria findSmallestPrefilteredTransactionList(const Filter& filter) const;

			// if called, mWorkMutex should be locked
			bool isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const;
			mutable std::recursive_mutex mWorkMutex;

			// update map and multimap on every transaction add and remove
			//! key is pubkey
			std::multimap<memory::ConstBlockPtr, std::shared_ptr<TransactionEntry>> mTransactionsByPubkey;
			//! key is transaction received date
			std::multimap<data::TimestampSeconds, std::shared_ptr<TransactionEntry>> mTransactionsByConfirmedAt;
			//! find transaction nr by iota message id
			std::unordered_map<iota::MessageId, uint64_t> mMessageIdTransactionNrs;
			//! find transactionEntry by transaction nr
			std::map<uint64_t, std::shared_ptr<TransactionEntry>> mTransactionsByNr;
			//! deferred transfers with redeem transfer if exist sorted by deferred transfer timeout
			std::multimap<Timepoint, DeferredRedeemedTransferPair> mTimeoutDeferredRedeemedTransferPairs;
			// for fast doublette check
			std::unordered_map<SignatureOctet, std::shared_ptr<TransactionEntry>> mTransactionFingerprintTransactionEntry;
			// because sorted transactions are not needed often, update list only if needed and mSortedDirty = true
			bool mSortedDirty;
			TransactionEntries mSortedTransactions;
			bool mExitCalled;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H