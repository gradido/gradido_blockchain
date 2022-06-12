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

	std::vector<Poco::SharedPtr<TransactionEntry>> TransactionsManagerBlockchain::searchTransactions(
		uint64_t startTransactionNr/* = 0*/,
		std::function<FilterResult(model::TransactionEntry*)> filter/* = nullptr*/,
		SearchDirection order /*= SearchDirection::ASC*/
	)
	{
		auto tm = TransactionsManager::getInstance();
		auto transactions = tm->getSortedTransactions(mGroupAlias);
		std::vector<Poco::SharedPtr<TransactionEntry>> result;
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
				Poco::SharedPtr<model::TransactionEntry> transactionEntry = new TransactionEntry(gradidoBlock.get());
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
				Poco::SharedPtr<model::TransactionEntry> transactionEntry = new TransactionEntry(gradidoBlock.get());
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

	Poco::SharedPtr<gradido::GradidoBlock> TransactionsManagerBlockchain::getLastTransaction(std::function<bool(const gradido::GradidoBlock*)> filter /*= nullptr*/)
	{
		auto tm = TransactionsManager::getInstance();
		auto transactions = tm->getSortedTransactions(mGroupAlias);
		auto lastTransaction = transactions->back();
		return createBlockFromTransaction(lastTransaction, transactions->size());
	}
	mpfr_ptr TransactionsManagerBlockchain::calculateAddressBalance(const std::string& address, const std::string& groupId, Poco::DateTime date)
	{
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
	Poco::SharedPtr<TransactionEntry> TransactionsManagerBlockchain::getTransactionForId(uint64_t transactionId)
	{
		throw std::runtime_error("not implemented yet");
	}
	Poco::SharedPtr<TransactionEntry> TransactionsManagerBlockchain::findLastTransactionForAddress(const std::string& address, const std::string& groupId /*= ""*/)
	{
		throw std::runtime_error("not implemented yet");
	}
	Poco::SharedPtr<TransactionEntry> TransactionsManagerBlockchain::findByMessageId(const MemoryBin* messageId, bool cachedOnly/* = true*/)
	{
		throw std::runtime_error("not implemented yet");
	}
	std::vector<Poco::SharedPtr<model::TransactionEntry>> TransactionsManagerBlockchain::findTransactions(const std::string& address, int month, int year)
	{
		auto tm = TransactionsManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, addressHex);

		std::vector<Poco::SharedPtr<model::TransactionEntry>> result;

		// TODO: is not current transaction nr, needs update for later use!
		int transactionNr = transactions.size();
		for (auto it = transactions.rbegin(); it != transactions.rend(); it++) {
			auto body = (*it)->getTransactionBody();
			auto received = body->getCreated();
			if (received.month() == month && received.year() == year) {
				auto gradidoBlock = createBlockFromTransaction(*it, transactionNr);
				Poco::SharedPtr<model::TransactionEntry> transactionEntry = new TransactionEntry(gradidoBlock.get());
				result.push_back(transactionEntry);
			}
			transactionNr--;
			if (received.year() < year) break;
			if (received.year() == year && received.month() < month) break;
		}
		return std::move(result);
	}
	void TransactionsManagerBlockchain::calculateCreationSum(const std::string& address, int month, int year, Poco::DateTime received, mpfr_ptr sum)
	{
		auto tm = TransactionsManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, addressHex);

		auto mm = MemoryManager::getInstance();
		//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
		auto amount = mm->getMathMemory();
		for (auto it = transactions.begin(); it != transactions.end(); it++) {
			auto body = (*it)->getTransactionBody();
			if(body->getCreated() > received) break;
			if (body->getTransactionType() == model::gradido::TRANSACTION_CREATION) {
				auto creation = body->getCreationTransaction();
				auto targetDate = creation->getTargetDate();
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

	const std::string& TransactionsManagerBlockchain::getGroupId() const
	{
		return mGroupAlias;
	}

	Poco::SharedPtr<model::gradido::GradidoBlock> TransactionsManagerBlockchain::createBlockFromTransaction(
		std::shared_ptr<model::gradido::GradidoTransaction> transaction, uint64_t transactionNr
	)
	{
		auto transactionCopy = std::make_unique<gradido::GradidoTransaction>(transaction->getSerialized().get());
		return gradido::GradidoBlock::create(
			std::move(transactionCopy),
			transactionNr,
			transaction->getTransactionBody()->getCreatedSeconds(),
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
