#ifndef __GRADIDO_BLOCKCHAIN_V3_3_IN_MEMORY_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_V3_3_IN_MEMORY_BLOCKCHAIN_H

#include "AbstractBlockchain.h"
#include "data/Protocol.h"
#include "gradido_blockchain/export.h"

#include <vector>

namespace gradido {
	namespace v3_3 {
		/*
		* @author einhornimmond
		* @date 16.06.2024
		* @brief  A easy system for holding transactions and check them
		* 
		* Not optimized for unlimited transactions, mostly used for testing, debugging and performance tests
		*/
		class GRADIDOBLOCKCHAIN_EXPORT InMemoryBlockchain : public AbstractBlockchain
		{
		public:
			InMemoryBlockchain(const std::string& communityId);
			~InMemoryBlockchain();

			// remove all transactions and start over
			void clear();

			void pushGradidoTransaction(std::shared_ptr<TransactionEntry> transactionEntry);
			void removeGradidoTransaction(std::shared_ptr<TransactionEntry> transactionEntry);

			struct UserBalance
			{
				UserBalance(memory::ConstBlockPtr _userPubkey, DecayDecimal _balance, Timepoint _balanceDate)
					: userPubkey(_userPubkey), balance(_balance), balanceDate(_balanceDate) {}
				memory::ConstBlockPtr userPubkey;
				DecayDecimal balance;
				Timepoint balanceDate;
			};

			UserBalance calculateUserBalanceUntil(memory::ConstBlockPtr userPubkey, Timepoint date);
			std::string getUserTransactionsDebugString(const std::string& groupAlias, const std::string& pubkeyHex);
			std::list<UserBalance> calculateFinalUserBalances(const std::string& groupAlias, Timepoint date);
			
			// get all transactions sorted by id
			const TransactionEntries& getSortedTransactions();

			// from Abstract blockchain
			TransactionEntries searchTransactions(
				uint64_t startTransactionNr = 0,
				std::function<FilterResult(TransactionEntry*)> filter = nullptr,
				SearchDirection order = SearchDirection::ASC
			);
			data::ConfirmedTransactionPtr getLastTransaction(std::function<bool(const data::ConfirmedTransaction&)> filter = nullptr);

			//! \param maxTransactionNr don't search after this transaction id, so maxTransactionNr is last transaction included in search, 0 for not filtering
			DecayDecimal calculateAddressBalance(
				memory::ConstBlockPtr accountPublicKey,
				const std::string& communityId,  // coin color
				Timepoint date,
				uint64_t maxTransactionNr = 0
			);
			//! \param maxTransactionNr don't search after this transaction id, so maxTransactionNr is last transaction included in search, 0 for not filtering
			data::AddressType getAddressType(memory::ConstBlockPtr accountPublicKey, uint64_t maxTransactionNr = 0);
			std::shared_ptr<TransactionEntry> getTransactionForId(uint64_t transactionId);
			//! \param coinGroupId last transaction for address filtered with coinCommunityId
			std::shared_ptr<TransactionEntry> findLastTransactionForAddress(
				memory::ConstBlockPtr accountPublicKey,
				const std::string& coinCommunityId = "",
				uint64_t maxTransactionNr = 0
			);
			std::shared_ptr<TransactionEntry> findByMessageId(memory::ConstBlockPtr messageId, bool cachedOnly = true);
			//! \brief Find every transaction belonging to address account in memory or block chain, expensive.
			//!
			//! Use with care, can need some time and return huge amount of data.
			//! \param address Address = user account public key.
			TransactionEntries findTransactions(memory::ConstBlockPtr accountPublicKey);
			//! \brief Find transactions of account from a specific month.
			//! \param address User account public key.
			//! \param maxTransactionNr don't search after this transaction id, so maxTransactionNr is last transaction included in search, 0 for not filtering
			TransactionEntries findTransactions(
				memory::ConstBlockPtr accountPublicKey,
				int month,
				int year,
				uint64_t maxTransactionNr = 0
			);
			const std::string& getCommunityId() const;

		protected:
			std::string mCommunityId;
			std::recursive_mutex mWorkMutex;

			// update map and multimap on every transaction add and remove
			//! key is pubkey
			std::multimap<memory::ConstBlockPtr, std::shared_ptr<TransactionEntry>> mTransactionsByPubkey;
			//! key is transaction received date
			std::multimap<data::TimestampSeconds, std::shared_ptr<TransactionEntry>> mTransactionsByConfirmedAt;
			// because sorted transactions are not needed often, update list only if needed and mSortedDirty = true
			bool mSortedDirty;
			TransactionEntries mSortedTransactions;
		};
	}

}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_IN_MEMORY_BLOCKCHAIN_H