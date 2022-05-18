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

	protected:
		TransactionsManager();

		// TODO: maybe we need a move constructor and to block copy constructor
		struct GroupTransactions
		{
			//! key is pubkey hex
			std::unordered_map<std::string, std::list<std::shared_ptr<model::gradido::GradidoTransaction>>> transactionsByPubkey;
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