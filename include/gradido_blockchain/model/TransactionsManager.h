#ifndef __GRADIDO_BLOCKCHAIN_TRANSACTIONS_MANAGER_H
#define __GRADIDO_BLOCKCHAIN_TRANSACTIONS_MANAGER_H

#include <unordered_map>
#include "../lib/MultithreadContainer.h"
#include "protobufWrapper/GradidoTransaction.h"

namespace model {

	/*!
		@author einhornimmond
		@date 18.05.22
		@brief A easy system for holding transactions and check them

		Not optimized for unlimited transactions, mostly used for testing, debugging and performance tests
	*/

	class TransactionsManager : protected MultithreadContainer
	{
	public:
		static TransactionsManager* getInstance();
		~TransactionsManager();

		void pushGradidoTransaction(const std::string& groupAlias, uint64_t id, std::unique_ptr<model::gradido::GradidoTransaction> transaction);
		
		struct UserBalance
		{
			UserBalance(const std::string _pubkeyHex, const std::string _balance, Poco::DateTime _balanceDate)
				: pubkeyHex(_pubkeyHex), balance(_balance), balanceDate(_balanceDate) {}
			std::string pubkeyHex;
			std::string balance;
			Poco::DateTime balanceDate;
		};

		UserBalance calculateUserBalanceUntil(const std::string& groupAlias, const std::string& pubkeyHex, Poco::DateTime date);
		std::string getUserTransactionsDebugString(const std::string& groupAlias, const std::string& pubkeyHex);
		std::list<UserBalance> calculateFinalUserBalances(const std::string& groupAlias, Poco::DateTime date);

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

	protected:
		TransactionsManager();
		typedef std::list<std::shared_ptr<model::gradido::GradidoTransaction>> TransactionList;
		TransactionList getSortedTransactionsForUser(const std::string& groupAlias, const std::string& pubkeyHex);
		

		// TODO: maybe we need a move constructor and to block copy constructor
		struct GroupTransactions
		{
			//! key is pubkey hex
			std::map<std::string, TransactionList> transactionsByPubkey;
			//! key is transaction nr or id
			std::map<uint64_t, std::shared_ptr<model::gradido::GradidoTransaction>> transactionsById;
		};
		//! contain all transactions
		//! first key is group alias
		//! second key is account address (pubkey in hex)
		std::unordered_map<std::string, GroupTransactions> mAllTransactions;
	};

}

#endif //__GRADIDO_BLOCKCHAIN_TRANSACTIONS_MANAGER_H