#ifndef __GRADIDO_BLOCKCHAIN_TRANSACTIONS_MANAGER_H
#define __GRADIDO_BLOCKCHAIN_TRANSACTIONS_MANAGER_H

#include <unordered_map>
#include <map>
#include "../lib/MultithreadContainer.h"
#include "protobufWrapper/GradidoTransaction.h"

namespace model {

	/*!
		@author einhornimmond
		@date 18.05.22
		@brief A easy system for holding transactions and check them

		Not optimized for unlimited transactions, mostly used for testing, debugging and performance tests
	*/

	class GRADIDOBLOCKCHAIN_EXPORT TransactionsManager final: protected MultithreadContainer
	{
	public:
		typedef std::list<std::shared_ptr<model::gradido::GradidoTransaction>> TransactionList;

		static TransactionsManager* getInstance();
		
		void clear();

		void pushGradidoTransaction(const std::string& groupAlias, std::unique_ptr<model::gradido::GradidoTransaction> transaction);
		void removeGradidoTransaction(const std::string& groupAlias, std::shared_ptr<model::gradido::GradidoTransaction> transaction);
		
		struct UserBalance
		{
			UserBalance(const std::string _pubkeyHex, const std::string _balance, Timepoint _balanceDate)
				: pubkeyHex(_pubkeyHex), balance(_balance), balanceDate(_balanceDate) {}
			std::string pubkeyHex;
			std::string balance;
			Timepoint balanceDate;
		};

		UserBalance calculateUserBalanceUntil(
			const std::string& groupAlias,
			const std::string& pubkeyHex, 
			Timepoint date
		);
		std::string getUserTransactionsDebugString(const std::string& groupAlias, const std::string& pubkeyHex);
		std::list<UserBalance> calculateFinalUserBalances(const std::string& groupAlias, Timepoint date);

		TransactionList getSortedTransactionsForUser(const std::string& groupAlias, const std::string& pubkeyHex);
		// get all transactions from a group sorted by id
		const TransactionList* getSortedTransactions(const std::string& groupAlias);

		class GroupNotFoundException : public GradidoBlockchainException
		{
		public:
			explicit GroupNotFoundException(const char* what, const std::string& groupAlias) noexcept
				: GradidoBlockchainException(what), mGroupAlias(groupAlias) {}
			std::string getFullString() const;
		protected:
			std::string mGroupAlias;
		};

		class AccountInGroupNotFoundException : public GradidoBlockchainException
		{
		public: 
			explicit AccountInGroupNotFoundException(const char* what, const std::string& groupAlias, const std::string& pubkeyHex) noexcept;
			std::string getFullString() const;
		protected:
			std::string mGroupAlias;
			std::string mPubkeyHex;
		};

		class MissingTransactionNrException : public GradidoBlockchainException
		{
		public:
			explicit MissingTransactionNrException(const char* what, uint64_t lastTransactionNr, uint64_t nextTransactionNr) noexcept;
			std::string getFullString() const;

		protected:
			uint64_t mLastTransactionNr;
			uint64_t mNextTransactionNr;
		};

	protected:
		
		
		Timepoint mDecayStartTime;

		// TODO: maybe we need a move constructor and to block copy constructor
		struct GroupTransactions
		{
			GroupTransactions() : dirty(true) {}
			~GroupTransactions() {
				sortedTransactions.clear();
				transactionsByReceived.clear();
				for (auto it = transactionsByPubkey.begin(); it != transactionsByPubkey.end(); it++) {
					it->second.clear();
				}
				transactionsByPubkey.clear();
			}
			//! key is pubkey hex
			std::map<std::string, TransactionList> transactionsByPubkey;
			//! key is transaction received date
			std::multimap<Timepoint, std::shared_ptr<model::gradido::GradidoTransaction>> transactionsByReceived;
			// cache of sorted transactions because traversing multimap shows on profiling as relatively slow
			bool dirty;
			TransactionList sortedTransactions;
		};
		//! contain all transactions
		//! first key is group alias
		//! second key is account address (pubkey in hex)
		std::unordered_map<std::string, GroupTransactions> mAllTransactions;

	private: 
		TransactionsManager();
		~TransactionsManager();

		/* Explicitly disallow copying. */
		TransactionsManager(const TransactionsManager&) = delete;
		TransactionsManager& operator= (const TransactionsManager&) = delete;
		
	};

}

#endif //__GRADIDO_BLOCKCHAIN_TRANSACTIONS_MANAGER_H