#include "date/date.h"

#include "gradido_blockchain/model/TransactionsManagerBlockchain.h"
#include "gradido_blockchain/model/TransactionsManager.h"
#include "gradido_blockchain/lib/Decay.h"

namespace model {
	TransactionsManagerBlockchain::TransactionsManagerBlockchain(const std::string& groupAlias)
		: mGroupAlias(groupAlias)
	{

	}

	TransactionsManagerBlockchain::~TransactionsManagerBlockchain()
	{

	}

	std::vector<std::shared_ptr<TransactionEntry>> TransactionsManagerBlockchain::searchTransactions(
		uint64_t startTransactionNr/* = 0*/,
		std::function<FilterResult(model::TransactionEntry*)> filter/* = nullptr*/,
		SearchDirection order /*= SearchDirection::ASC*/
	)
	{
		auto tm = TransactionsManager::getInstance();
		auto transactions = tm->getSortedTransactions(mGroupAlias);
		std::vector<std::shared_ptr<TransactionEntry>> result;
		uint64_t transactionNr = 1;
		if (startTransactionNr > transactions->size()) {
			throw TransactionsCountException("not enough transactions in blockchain", startTransactionNr, transactions->size());
		}

		if (SearchDirection::ASC == order) {
			auto it = transactions->begin();
			std::advance(it, startTransactionNr-1);
			transactionNr = startTransactionNr;
			for (; it != transactions->end(); ++it) {
				auto gradidoBlock = createBlockFromTransaction(*it, transactionNr);
				std::shared_ptr<model::TransactionEntry> transactionEntry = std::make_shared<model::TransactionEntry>(gradidoBlock.get());
				FilterResult filterTransactionResult = FilterResult::USE;
				if (filter) {
					filterTransactionResult = filter(transactionEntry.get());
				}
				if (FilterResult::USE == (filterTransactionResult & FilterResult::USE)) {
					result.push_back(transactionEntry);
				}
				if (FilterResult::STOP == (filterTransactionResult & FilterResult::STOP)) {
					return std::move(result);
				}
				transactionNr++;
			}
			return std::move(result);
		}
		else if (SearchDirection::DESC == order) {
			transactionNr = transactions->size();
			for (auto it = transactions->rbegin(); it != transactions->rend(); ++it) {
				auto gradidoBlock = createBlockFromTransaction(*it, transactionNr);
				std::shared_ptr<model::TransactionEntry> transactionEntry = std::make_shared<model::TransactionEntry>(gradidoBlock.get());
				FilterResult filterTransactionResult = FilterResult::USE;
				if (filter) {
					filterTransactionResult = filter(transactionEntry.get());
				}
				if (FilterResult::USE == (filterTransactionResult & FilterResult::USE)) {
					result.push_back(transactionEntry);
				}
				if (FilterResult::STOP == (filterTransactionResult & FilterResult::STOP)) {
					return std::move(result);
				}
				transactionNr--;
			}
		}
		return std::move(result);
	}

	std::shared_ptr<gradido::ConfirmedTransaction> TransactionsManagerBlockchain::getLastTransaction(std::function<bool(const gradido::ConfirmedTransaction*)> filter /*= nullptr*/)
	{
		if (filter) {
			throw std::runtime_error("[TransactionsManagerBlockchain::getLastTransaction] not implemented with filter yet");
		}
		auto tm = TransactionsManager::getInstance();
		auto transactions = tm->getSortedTransactions(mGroupAlias);
		auto lastTransaction = transactions->back();
		return createBlockFromTransaction(lastTransaction, transactions->size());
	}
	mpfr_ptr TransactionsManagerBlockchain::calculateAddressBalance(
		const std::string& address, 
		const std::string& groupId, 
		Timepoint date,
		uint64_t ownTransactionNr
	)
	{
		// TODO: limit search to not include transaction with nr ownTransactionNr
		auto tm = TransactionsManager::getInstance();
		auto mm = MemoryManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto balance = tm->calculateUserBalanceUntil(mGroupAlias, addressHex, date);

		auto gdd = mm->getMathMemory();
		mpfr_set_str(gdd, balance.balance.data(), 10, gDefaultRound);

		return gdd;
	}
	proto::gradido::RegisterAddress_AddressType TransactionsManagerBlockchain::getAddressType(const std::string& address)
	{
		auto tm = TransactionsManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, addressHex);
		auto firstTransaction = transactions.front();
		auto body = firstTransaction->getTransactionBody();
		if (body->isRegisterAddress()) {
			auto registerAddress = body->getRegisterAddress();
			return registerAddress->getAddressType();
		}
		return proto::gradido::RegisterAddress_AddressType_NONE;
	}
	std::shared_ptr<TransactionEntry> TransactionsManagerBlockchain::getTransactionForId(uint64_t transactionId)
	{
		throw std::runtime_error("not implemented yet");
	}
	std::shared_ptr<TransactionEntry> TransactionsManagerBlockchain::findLastTransactionForAddress(const std::string& address, const std::string& groupId /*= ""*/)
	{
		auto tm = TransactionsManager::getInstance();
		auto pubkeyHex = DataTypeConverter::binToHex(address);
		try {
			auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, pubkeyHex);
			auto lastTransaction = transactions.back();
			auto gradidoBlock = createBlockFromTransaction(lastTransaction, transactions.size());
			std::shared_ptr<model::TransactionEntry> transactionEntry = std::make_shared<model::TransactionEntry>(gradidoBlock.get());
			return transactionEntry;
		}
		// not ideal, because exceptions should not be thrown in regular program operation
		catch (TransactionsManager::AccountInGroupNotFoundException& ex) {
			return nullptr;
		}
	}
	std::shared_ptr<TransactionEntry> TransactionsManagerBlockchain::findByMessageId(const MemoryBin* messageId, bool cachedOnly/* = true*/)
	{
		throw std::runtime_error("not implemented yet");
	}
	std::vector<std::shared_ptr<TransactionEntry>> TransactionsManagerBlockchain::findTransactions(const std::string& address)
	{
		throw std::runtime_error("not implemented yet");
	}
	std::vector<std::shared_ptr<model::TransactionEntry>> TransactionsManagerBlockchain::findTransactions(const std::string& address, int _month, int _year)
	{
		auto tm = TransactionsManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, addressHex);
		date::month month(_month);
		date::year year(_year);

		std::vector<std::shared_ptr<model::TransactionEntry>> result;

		// TODO: is not current transaction nr, needs update for later use!
		int transactionNr = transactions.size();
		for (auto it = transactions.rbegin(); it != transactions.rend(); it++) {
			auto body = (*it)->getTransactionBody();
			auto received = date::year_month_day{ date::floor<date::days>(body->getCreatedAt()) };
			if (received.month() == month && received.year() == year) {
				auto gradidoBlock = createBlockFromTransaction(*it, transactionNr);
				std::shared_ptr<model::TransactionEntry> transactionEntry = std::make_shared<model::TransactionEntry>(gradidoBlock.get());
				result.push_back(transactionEntry);
			}
			transactionNr--;
			if (received.year() < year) break;
			if (received.year() == year && received.month() < month) break;
		}
		return std::move(result);
	}
	void TransactionsManagerBlockchain::calculateCreationSum(
		const std::string& address,
		int _month, int _year,
		Timepoint received,
		mpfr_ptr sum
	)
	{
		auto tm = TransactionsManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, addressHex);
		date::month month(_month);
		date::year year(_year);

		auto mm = MemoryManager::getInstance();
		//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
		auto amount = mm->getMathMemory();
		for (auto it = transactions.begin(); it != transactions.end(); it++) {
			auto body = (*it)->getTransactionBody();
			if(body->getCreatedAt() > received) break;
			if (body->getTransactionType() == model::gradido::TRANSACTION_CREATION) {
				auto creation = body->getCreationTransaction();
				auto targetDate = date::year_month_day{ date::floor<date::days>(creation->getTargetDate()) };
				if (targetDate.month() != month || targetDate.year() != year) {
					continue;
				}
				//printf("added from transaction: %d \n", gradidoBlock->getID());
				mpfr_set_str(amount, creation->getAmount().data(), 10, gDefaultRound);
				mpfr_add(sum, sum, amount, gDefaultRound);
			}
		}
		mm->releaseMathMemory(amount);
		// TODO: if user has moved from another blockchain, get also creation transactions from origin group, recursive
		// TODO: check also address type, because only for human account creation is allowed
		// New idea from Bernd: User can be in multiple groups gather creations in different groups in different colors
		// maybe using a link transaction
	}

	const std::string& TransactionsManagerBlockchain::getCommunityId() const
	{
		return mGroupAlias;
	}

	std::shared_ptr<model::gradido::ConfirmedTransaction> TransactionsManagerBlockchain::createBlockFromTransaction(
		std::shared_ptr<model::gradido::GradidoTransaction> transaction, uint64_t transactionNr
	)
	{
		auto transactionCopy = std::make_unique<gradido::GradidoTransaction>(transaction->getSerialized().get());
		return gradido::ConfirmedTransaction::create(
			std::move(transactionCopy),
			transactionNr,
			transaction->getTransactionBody()->getCreatedAtSeconds(),
			nullptr
		);
	}

	// *************************** Exceptions ****************************************
	TransactionsCountException::TransactionsCountException(const char* what, uint64_t expectedTransactionsCount, uint64_t transactionsCount) noexcept
		: GradidoBlockchainException(what), mExpectedTransactionsCount(expectedTransactionsCount), mTransactionsCount(transactionsCount)
	{

	}

	std::string TransactionsCountException::getFullString() const
	{
		std::string result = what();
		result += ", expected transactions count: " + std::to_string(mExpectedTransactionsCount);
		result += ", transactions count: " + std::to_string(mTransactionsCount);
		return result;
	}
}
