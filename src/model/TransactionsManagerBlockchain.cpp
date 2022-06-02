#include "gradido_blockchain/model/TransactionsManagerBlockchain.h"
#include "gradido_blockchain/model/TransactionsManager.h"
#include "gradido_blockchain/lib/Decay.h"

namespace model {
	TransactionsManagerBlockchain::TransactionsManagerBlockchain(const std::string& groupAlias)
		: mGroupAlias(groupAlias)
	{

	}

	std::vector<Poco::SharedPtr<TransactionEntry>> TransactionsManagerBlockchain::getAllTransactions(std::function<bool(model::TransactionEntry*)> filter/* = nullptr*/)
	{
		throw std::runtime_error("not implemented yet");
	}

	Poco::SharedPtr<gradido::GradidoBlock> TransactionsManagerBlockchain::getLastTransaction(std::function<bool(const gradido::GradidoBlock*)> filter /*= nullptr*/)
	{
		auto tm = TransactionsManager::getInstance();
		auto transactions = tm->getSortedTransactions(mGroupAlias);
		auto lastTransaction = transactions.back();
		auto lastTransactionCopy = std::make_unique<gradido::GradidoTransaction>(lastTransaction->getSerialized().get());
		auto result = gradido::GradidoBlock::create(
			std::move(lastTransactionCopy),
			transactions.size(),
			lastTransaction->getTransactionBody()->getCreatedSeconds(),
			nullptr
		);
		return result;
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
	void TransactionsManagerBlockchain::calculateCreationSum(const std::string& address, int month, int year, Poco::DateTime received, mpfr_ptr sum)
	{
		auto tm = TransactionsManager::getInstance();
		auto addressHex = DataTypeConverter::binToHex(address).substr(0, 64);
		auto transactions = tm->getSortedTransactionsForUser(mGroupAlias, addressHex);
		
		// received = max
		// received - 2 month = min
		
		auto mm = MemoryManager::getInstance();
		//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
		auto amount = mm->getMathMemory();
		for (auto it = transactions.begin(); it != transactions.end(); it++) {
			auto body = (*it)->getTransactionBody();
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

}