#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/TransactionsManager.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/DecayDecimal.h"

#include <algorithm>

namespace model {

	TransactionsManager::TransactionsManager()
	{
		std::string decayStartTimeString = "2021-05-13 17:46:31";
		int timezoneDifferential = Poco::DateTimeFormatter::UTC; // + GMT 0
		mDecayStartTime = Poco::DateTimeParser::parse(
			Poco::DateTimeFormat::SORTABLE_FORMAT,
			decayStartTimeString,
			timezoneDifferential
		);
	}

	TransactionsManager::~TransactionsManager()
	{
		printf("[TransactionsManager::~TransactionsManager]\n");
	}

	void TransactionsManager::clear()
	{
		mAllTransactions.clear();
	}

	TransactionsManager* TransactionsManager::getInstance()
	{
		static TransactionsManager one;
		return &one;
	}

	void TransactionsManager::pushGradidoTransaction(const std::string& groupAlias, std::unique_ptr<model::gradido::GradidoTransaction> transaction)
	{
		std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);
		auto mm = MemoryManager::getInstance();

		auto groupTransactionsIt = mAllTransactions.find(groupAlias);
		if (groupTransactionsIt == mAllTransactions.end()) {
			auto result = mAllTransactions.insert({ groupAlias, GroupTransactions() });
			groupTransactionsIt = result.first;
		}
		groupTransactionsIt->second.dirty = true;
		auto sharedTransaction = std::shared_ptr<model::gradido::GradidoTransaction>(transaction.release());
		auto transactionBody = sharedTransaction->getTransactionBody();
		groupTransactionsIt->second.transactionsByReceived.insert({
			transactionBody->getCreated(),
			sharedTransaction
		});
		groupTransactionsIt->second.dirty = true;
		auto involvedAddresses = transactionBody->getTransactionBase()->getInvolvedAddresses();
		if (transactionBody->isTransfer() && involvedAddresses.size() != 2) {
			throw std::runtime_error("transfer transaction hasn't two involved addresses");
		}
		for (auto it = involvedAddresses.begin(); it != involvedAddresses.end(); it++) {
			auto pubkeyHex = (*it)->convertToHex().get()->substr(0,64);
			mm->releaseMemory(*it);
			auto pubkeyTransactionsIt = groupTransactionsIt->second.transactionsByPubkey.find(pubkeyHex);
			if (pubkeyTransactionsIt == groupTransactionsIt->second.transactionsByPubkey.end()) {
				auto result = groupTransactionsIt->second.transactionsByPubkey.insert({
					pubkeyHex,
					std::list<std::shared_ptr<model::gradido::GradidoTransaction>>()
				});
				pubkeyTransactionsIt = result.first;
			}
			pubkeyTransactionsIt->second.push_back(sharedTransaction);
			//groupTransactionsIt->second.transactionsByPubkey.insert({})
		}
		involvedAddresses.clear();
	}

	void TransactionsManager::removeGradidoTransaction(const std::string& groupAlias, std::shared_ptr<model::gradido::GradidoTransaction> transaction)
	{
		std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);
		auto mm = MemoryManager::getInstance();

		auto groupTransactionsIt = mAllTransactions.find(groupAlias);
		if (groupTransactionsIt == mAllTransactions.end()) {
			throw GroupNotFoundException("[TransactionsManager::removeGradidoTransaction]", groupAlias);
		}
		auto byReceivedIt = groupTransactionsIt->second.transactionsByReceived.equal_range(transaction->getTransactionBody()->getCreated());
		auto transactionBodyBytes = transaction->getTransactionBody()->getBodyBytes();
		for (auto it = byReceivedIt.first; it != byReceivedIt.second; ++it)
		{
			auto bodyBytes = it->second->getTransactionBody()->getBodyBytes();
			// 0 means equal
			if (transactionBodyBytes->compare(*bodyBytes) == 0) {
				it = groupTransactionsIt->second.transactionsByReceived.erase(it);
				if (it == byReceivedIt.second) {
					break;
				}
				//break;
			}
		}
		auto involvedAddresses = transaction->getTransactionBody()->getTransactionBase()->getInvolvedAddresses();
		for (auto iit = involvedAddresses.begin(); iit != involvedAddresses.end(); iit++) {
			auto pubkeyHex = DataTypeConverter::binToHex(*iit);
			mm->releaseMemory(*iit);
			auto byPubkey = groupTransactionsIt->second.transactionsByPubkey.find(pubkeyHex.substr(0, 64));
			for (auto it = byPubkey->second.begin(); it != byPubkey->second.end(); it++) {
				auto bodyBytes = (*it)->getTransactionBody()->getBodyBytes();
				if (transactionBodyBytes->compare(*bodyBytes) == 0) {
					it = byPubkey->second.erase(it);
					if (it == byPubkey->second.end()) {
						break;
					}
				}
			}
		}
		groupTransactionsIt->second.dirty = true;

	}

	TransactionsManager::UserBalance TransactionsManager::calculateUserBalanceUntil(const std::string& groupAlias, const std::string& pubkeyHex, Poco::DateTime date)
	{
		auto transactions = getSortedTransactionsForUser(groupAlias, pubkeyHex);
		auto mm = MemoryManager::getInstance();
		DecayDecimal balance;
		DecayDecimal amount;
		auto pubkeyBinString = DataTypeConverter::hexToBinString(pubkeyHex.substr(0, 64));

		Poco::DateTime now;
		Poco::DateTime lastBalanceDate(now);
		for (auto it = transactions.begin(); it != transactions.end(); it++) {
			auto transactionBody = (*it)->getTransactionBody();
			Poco::DateTime localDate = Poco::Timestamp((Poco::UInt64)transactionBody->getCreatedSeconds() * Poco::Timestamp::resolution());
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

	std::string TransactionsManager::getUserTransactionsDebugString(const std::string& groupAlias, const std::string& pubkeyHex)
	{
		auto transactions = getSortedTransactionsForUser(groupAlias, pubkeyHex);
		std::string result;
		auto mm = MemoryManager::getInstance();
		auto balance = mm->getMathMemory();
		auto amount = mm->getMathMemory();
		auto decayForDuration = mm->getMathMemory();
		std::string balanceString;

		Poco::DateTime now;
		Poco::DateTime lastBalanceDate(now);
		for (auto it = transactions.begin(); it != transactions.end(); it++) {
			auto transactionBody = (*it)->getTransactionBody();
			Poco::DateTime createdDate = Poco::Timestamp((Poco::UInt64)transactionBody->getCreatedSeconds() * Poco::Timestamp::resolution());
			if (lastBalanceDate != now && createdDate != lastBalanceDate) {
				// gDecayFactorGregorianCalender
				// gDecayFactor356Days
				calculateDecayFactorForDuration(decayForDuration, gDecayFactorGregorianCalender, lastBalanceDate, createdDate);
				std::string decayForDurationString;
				model::gradido::TransactionBase::amountToString(&decayForDurationString, decayForDuration);
				Poco::Timespan duration = createdDate - lastBalanceDate;
				std::string durationString = Poco::DateTimeFormatter::format(duration);
				mpfr_set(amount, balance, gDefaultRound);
				calculateDecayFast(decayForDuration, balance);
				mpfr_sub(amount, amount, balance, gDefaultRound);
				result += "decay duration: " + durationString + ", factor: " + decayForDurationString;
				char buffer[256]; memset(buffer, 0, 256);
				mpfr_sprintf(buffer, ", decayed: %.5Rf\n", amount);
				result += buffer;
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
				auto senderPubkeyHex = DataTypeConverter::binToHex(transfer->getSenderPublicKeyString()).substr(0,64);
				if (senderPubkeyHex == pubkeyHex) {
					subtract = true;
				}
			}
			else if (transactionBody->isRegisterAddress()) {
			}
			mpfr_set_str(amount, amountString.data(), 10, gDefaultRound);
			if (subtract) {
				mpfr_sub(balance, balance, amount, gDefaultRound);
			}
			else {
				mpfr_add(balance, balance, amount, gDefaultRound);
			}
			model::gradido::TransactionBase::amountToString(&balanceString, balance);
			result += transactionBody->toDebugString();
			result += "balance: " + balanceString + " GDD\n";
			result += "\n";
			lastBalanceDate = createdDate;
		}
		result.erase(std::remove(result.begin(), result.end(), '\0'), result.end());
		mm->releaseMathMemory(balance);
		mm->releaseMathMemory(amount);
		mm->releaseMathMemory(decayForDuration);
		return std::move(result);
	}

	std::list<TransactionsManager::UserBalance> TransactionsManager::calculateFinalUserBalances(const std::string& groupAlias, Poco::DateTime date)
	{
		auto it = mAllTransactions.find(groupAlias);
		if (it == mAllTransactions.end()) {
			throw GroupNotFoundException("[TransactionsManager::getSortedTransactionsForUser]", groupAlias);
		}
		std::list<TransactionsManager::UserBalance> resultList;
		std::for_each(
			it->second.transactionsByPubkey.begin(),
			it->second.transactionsByPubkey.end(),
			[&](const std::pair<std::string, TransactionList>& pair) {
				resultList.push_back(std::move(calculateUserBalanceUntil(groupAlias, pair.first, date)));
			}
		);
		return std::move(resultList);
	}

	std::string TransactionsManager::GroupNotFoundException::getFullString() const
	{
		std::string result = what();
		result += ", group alias: " + mGroupAlias;
		return std::move(result);
	}

	TransactionsManager::AccountInGroupNotFoundException::AccountInGroupNotFoundException(const char* what, const std::string& groupAlias, const std::string& pubkeyHex) noexcept
		: GradidoBlockchainException(what), mGroupAlias(groupAlias), mPubkeyHex(pubkeyHex)
	{

	}

	std::string TransactionsManager::AccountInGroupNotFoundException::getFullString() const
	{
		std::string result = what();
		result += ", group alias: " + mGroupAlias;
		result += ", pubkey: " + mPubkeyHex;
		return std::move(result);
	}

	TransactionsManager::MissingTransactionNrException::MissingTransactionNrException(const char* what, uint64_t lastTransactionNr, uint64_t nextTransactionNr) noexcept
		: GradidoBlockchainException(what), mLastTransactionNr(lastTransactionNr), mNextTransactionNr(nextTransactionNr)
	{

	}

	std::string TransactionsManager::MissingTransactionNrException::getFullString() const
	{
		std::string result = what();
		result += ", hole between transaction nr: " + std::to_string(mLastTransactionNr);
		result += " and transaction nr: " + std::to_string(mNextTransactionNr);
		return result;
	}

	TransactionsManager::TransactionList TransactionsManager::getSortedTransactionsForUser(const std::string& groupAlias, const std::string& pubkeyHex)
	{
		auto it = mAllTransactions.find(groupAlias);
		if (it == mAllTransactions.end()) {
			throw GroupNotFoundException("[TransactionsManager::getSortedTransactionsForUser]", groupAlias);
		}
		auto accountIt = it->second.transactionsByPubkey.find(pubkeyHex);
		if (accountIt == it->second.transactionsByPubkey.end()) {
			throw AccountInGroupNotFoundException("[TransactionsManager::getSortedTransactionsForUser]", groupAlias, pubkeyHex);
		}
		TransactionList resultList(accountIt->second);
		resultList.sort([](
			const std::shared_ptr<model::gradido::GradidoTransaction>&a,
			const std::shared_ptr<model::gradido::GradidoTransaction>&b
			) {
			return a->getTransactionBody()->getCreatedSeconds() < b->getTransactionBody()->getCreatedSeconds();
		});
		return std::move(resultList);
	}

	const TransactionsManager::TransactionList* TransactionsManager::getSortedTransactions(const std::string& groupAlias)
	{
		std::scoped_lock _lock(mWorkMutex);
		auto itGroup = mAllTransactions.find(groupAlias);
		if (itGroup == mAllTransactions.end()) {
			throw GroupNotFoundException("[TransactionsManager::getSortedTransactionsForUser]", groupAlias);
		}

		auto transactionsByReceived = &itGroup->second.transactionsByReceived;
		if (itGroup->second.dirty) {
			itGroup->second.sortedTransactions.clear();
			for (auto it = transactionsByReceived->begin(); it != transactionsByReceived->end(); it++) {
				itGroup->second.sortedTransactions.push_back(it->second);
			}
			itGroup->second.dirty = false;
		}
		return &itGroup->second.sortedTransactions;
		//TransactionList resultList(itGroup->second.sortedTransactions);
		//return std::move(resultList);
	}

}
