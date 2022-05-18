#include "gradido_blockchain/model/TransactionsManager.h"
#include "gradido_blockchain/MemoryManager.h"

namespace model {

	TransactionsManager::TransactionsManager()
	{

	}

	TransactionsManager::~TransactionsManager()
	{

	}

	TransactionsManager* TransactionsManager::getInstance()
	{
		static TransactionsManager one;
		return &one;
	}

	void TransactionsManager::pushGradidoTransaction(const std::string& groupAlias, uint64_t id, std::unique_ptr<model::gradido::GradidoTransaction> transaction)
	{
		std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);
		auto mm = MemoryManager::getInstance();

		auto groupTransactionsIt = mAllTransactions.find(groupAlias);
		if (groupTransactionsIt == mAllTransactions.end()) {
			auto result = mAllTransactions.insert({ groupAlias, GroupTransactions() });
			groupTransactionsIt = result.first;
		}
		auto sharedTransaction = std::shared_ptr<model::gradido::GradidoTransaction>(transaction.release());
		groupTransactionsIt->second.transactionsById.insert({ id, sharedTransaction });
		auto involvedAddresses = sharedTransaction->getTransactionBody()->getTransactionBase()->getInvolvedAddresses();
		for (auto it = involvedAddresses.begin(); it != involvedAddresses.end(); it++) {
			auto pubkeyHex = (*it)->convertToHex();
			mm->releaseMemory(*it);
			auto pubkeyTransactionsIt = groupTransactionsIt->second.transactionsByPubkey.find(*pubkeyHex.get());
			if (pubkeyTransactionsIt == groupTransactionsIt->second.transactionsByPubkey.end()) {
				auto result = groupTransactionsIt->second.transactionsByPubkey.insert({
					std::move(*pubkeyHex.release()),
					std::list<std::shared_ptr<model::gradido::GradidoTransaction>>()
					});
				pubkeyTransactionsIt = result.first;
			}
			pubkeyTransactionsIt->second.push_back(sharedTransaction);
			//groupTransactionsIt->second.transactionsByPubkey.insert({})
		}
		involvedAddresses.clear();
	}

}