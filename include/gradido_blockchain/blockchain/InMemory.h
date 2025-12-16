#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H

#include "Abstract.h"
#include "FilterResult.h"
#include "TransactionsIndex.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/BlockKey.h"

#include <vector>
#include <unordered_map>
#include <shared_mutex>		

namespace gradido {
	namespace data {
		class AccountBalance;
	}
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
				const data::LedgerAnchor& ledgerAnchor,
				data::Timestamp confirmedAt
			) override;
			bool createAndAddConfirmedTransactionExtern(
				data::ConstGradidoTransactionPtr gradidoTransaction,
				uint64_t legacyTransactionNr,
				std::vector<data::AccountBalance>&& accountBalances
			);
			virtual void addTransactionTriggerEvent(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent) override;
			virtual void removeTransactionTriggerEvent(const data::TransactionTriggerEvent& transactionTriggerEvent) override;

			virtual bool isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const override;

			//! return events in asc order of targetDate
			virtual std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> findTransactionTriggerEventsInRange(TimepointInterval range) override;
			virtual std::shared_ptr<const data::TransactionTriggerEvent> findNextTransactionTriggerEventInRange(TimepointInterval range) override;

			// get all transactions sorted by id
			const TransactionEntries& getSortedTransactions();

			// from Abstract blockchain
			TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const override;
			ConstTransactionEntryPtr findOne(const Filter& filter = Filter::LAST_TRANSACTION) const override;
			data::AddressType getAddressType(const Filter& filter = Filter::ALL_TRANSACTIONS) const override;

			ConstTransactionEntryPtr getTransactionForId(uint64_t transactionId) const;

			// this implementation use a map for direct search and don't use filter at all
			ConstTransactionEntryPtr findByLedgerAnchor(
				const data::LedgerAnchor& ledgerAnchor,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const override;

			AbstractProvider* getProvider() const;

		protected:
			InMemory(std::string_view communityId);

			TransactionsIndex mTransactionsIndex;

			// if called, mWorkMutex should be locked exclusive
			void pushTransactionEntry(ConstTransactionEntryPtr transactionEntry);
			void removeTransactionEntry(ConstTransactionEntryPtr transactionEntry);
			
			mutable std::recursive_mutex mWorkMutex;

			// update map and multimap on every transaction add and remove
			//! find transaction nr by hiero transaction id
			std::unordered_map<data::LedgerAnchor, uint64_t> mLedgerAnchorTransactionNrs;
			//! find transactionEntry by transaction nr
			std::map<uint64_t, ConstTransactionEntryPtr> mTransactionsByNr;
			// for fast doublette check
			std::unordered_multimap<SignatureOctet, ConstTransactionEntryPtr> mTransactionFingerprintTransactionEntry;
			// transactionTriggerEvents
			mutable std::mutex mTransactionTriggerEventsMutex;
			std::multimap<Timepoint, std::shared_ptr<const data::TransactionTriggerEvent>> mTransactionTriggerEvents;
			// because sorted transactions are not needed often, update list only if needed and mSortedDirty = true
			bool mSortedDirty;
			TransactionEntries mSortedTransactions;
			bool mExitCalled;
			ConstTransactionEntryPtr mLastTransaction;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_H