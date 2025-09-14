#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H

#include "Abstract.h"
#include "FilterResult.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/BlockKey.h"

#include <vector>
#include <unordered_map>
#include <shared_mutex>		

namespace gradido {
	namespace blockchain {
		class InMemoryProvider;
		/*
		* @author einhornimmond
		* @date 16.06.2024
		* @brief  A easy system for holding transactions and check them
		*
		* Not optimized for unlimited transactions, mostly used for testing, debugging and performance tests
		*/
		class GRADIDOBLOCKCHAIN_EXPORT InMemory : public Abstract
		{
			friend InMemoryProvider;
		public:
			
			~InMemory();

			// remove all transactions and start over
			void clear();
			void exit();

			//! validate and generate confirmed transaction
			//! throw if gradido transaction isn't valid
			//! \return false if transaction already exist
			virtual bool createAndAddConfirmedTransaction(
				data::ConstGradidoTransactionPtr gradidoTransaction,
				memory::ConstBlockPtr messageId, 
				Timepoint confirmedAt
			);
			virtual void addTransactionTriggerEvent(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent);
			virtual void removeTransactionTriggerEvent(const data::TransactionTriggerEvent& transactionTriggerEvent);

			virtual bool isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const;

			//! return events in asc order of targetDate
			virtual std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> findTransactionTriggerEventsInRange(TimepointInterval range);

			// get all transactions sorted by id
			const TransactionEntries& getSortedTransactions();

			// from Abstract blockchain
			TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const;

			std::shared_ptr<const TransactionEntry> getTransactionForId(uint64_t transactionId) const;

			// this implementation use a map for direct search and don't use filter at all
			std::shared_ptr<const TransactionEntry> findByMessageId(
				memory::ConstBlockPtr messageId,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const;

			AbstractProvider* getProvider() const;

		protected:
			InMemory(std::string_view communityId);

			// if called, mWorkMutex should be locked exclusive
			void pushTransactionEntry(std::shared_ptr<const TransactionEntry> transactionEntry);
			void removeTransactionEntry(std::shared_ptr<const TransactionEntry> transactionEntry);

			FilterCriteria findSmallestPrefilteredTransactionList(const Filter& filter) const;

			mutable std::recursive_mutex mWorkMutex;

			// update map and multimap on every transaction add and remove
			//! key is hash from pubkey, not collision resistent!
			std::multimap<memory::BlockKey, std::shared_ptr<const TransactionEntry>> mTransactionsByPubkey;
			//! key is transaction received date
			std::multimap<data::Timestamp, std::shared_ptr<const TransactionEntry>> mTransactionsByConfirmedAt;
			//! find transaction nr by hiero transaction id
			std::unordered_map<hiero::TransactionId, uint64_t, hiero::TransactionIdHasher> mHieroTransactionIdTransactionNrs;
			//! find transactionEntry by transaction nr
			std::map<uint64_t, std::shared_ptr<const TransactionEntry>> mTransactionsByNr;
			// for fast doublette check
			std::unordered_map<SignatureOctet, std::shared_ptr<const TransactionEntry>> mTransactionFingerprintTransactionEntry;
			// transactionTriggerEvents
			mutable std::mutex mTransactionTriggerEventsMutex;
			std::multimap<Timepoint, std::shared_ptr<const data::TransactionTriggerEvent>> mTransactionTriggerEvents;
			// because sorted transactions are not needed often, update list only if needed and mSortedDirty = true
			bool mSortedDirty;
			TransactionEntries mSortedTransactions;
			bool mExitCalled;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H