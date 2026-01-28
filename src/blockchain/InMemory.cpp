#include "gradido_blockchain/blockchain/InMemory.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/confirmTransaction/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Type.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <string_view>

using namespace magic_enum;
using std::string_view;
using std::lock_guard;
using memory::ConstBlockPtr, memory::Block;

namespace gradido {

	using data::AddressType, data::ConstGradidoTransactionPtr, data::Timestamp, data::LedgerAnchor, data::AccountBalance;
	using namespace interaction;

	namespace blockchain {
		InMemory::InMemory(string_view uniqueCommunityAlias, uint32_t communityIdIndex)
			: Abstract(communityIdIndex),
			mPublicKeyDirectory(std::string(uniqueCommunityAlias) + std::string("_publicKeyDictionary")),
			mTransactionsIndex(), mSortedDirty(false), mExitCalled(false)
		{

		}

		InMemory::~InMemory()
		{
		}

		void InMemory::clear()
		{
			lock_guard _lock(mWorkMutex);
			mSortedDirty = false;
			mSortedTransactions.clear();
			mTransactionsIndex.reset();
			mPublicKeyDirectory.reset();
		}

		void InMemory::exit()
		{
			lock_guard _lock(mWorkMutex);
			mExitCalled = true;
		}

		bool InMemory::createAndAddConfirmedTransaction(
			ConstGradidoTransactionPtr gradidoTransaction,
			const data::LedgerAnchor& ledgerAnchor,
			Timestamp confirmedAt
		) {
			auto blockchain = getProvider()->findBlockchain(mCommunityIdIndex);
			confirmTransaction::Context context(blockchain);
			auto role = context.createRole(gradidoTransaction, ledgerAnchor, confirmedAt);
			if (!role) {
				throw GradidoNotImplementedException("missing role for gradido transaction");
			}
			auto confirmedTransaction = context.run(role);
			if (!confirmedTransaction) {
				throw GradidoNullPointerException(
					"empty confirmed transaction from confirmTransaction interaction", 
					"data::ConfirmedTransaction",
					__FUNCTION__
				);
			}
			auto transactionEntry = std::make_shared<TransactionEntry>(confirmedTransaction, mCommunityIdIndex);
			pushTransactionEntry(transactionEntry);
			mTransactionFingerprintTransactionEntry.insert({ *confirmedTransaction->getGradidoTransaction()->getFingerprint(), transactionEntry });
			role->runPastAddToBlockchain(confirmedTransaction, blockchain);
			return true;
		}

		bool InMemory::createAndAddConfirmedTransactionExtern(
			data::ConstGradidoTransactionPtr gradidoTransaction,
			const data::LedgerAnchor& ledgerAnchor,
			std::vector<data::AccountBalance> accountBalances
		) {
			auto blockchain = getProvider()->findBlockchain(mCommunityIdIndex);
			confirmTransaction::Context context(blockchain);
			auto role = context.createRole(
				gradidoTransaction,
				ledgerAnchor,
				gradidoTransaction->getTransactionBody()->getCreatedAt()
			);
			if (!role) {
				throw GradidoNotImplementedException("missing role for gradido transaction");
			}
			role->setAccountBalances(accountBalances);
			auto confirmedTransaction = context.run(role);
			if (!confirmedTransaction) {
				throw GradidoNullPointerException(
					"empty confirmed transaction from confirmTransaction interaction",
					"data::ConfirmedTransaction",
					__FUNCTION__
				);
			}
			auto transactionEntry = std::make_shared<TransactionEntry>(confirmedTransaction, mCommunityIdIndex);
			pushTransactionEntry(transactionEntry);
			mTransactionFingerprintTransactionEntry.insert({ *confirmedTransaction->getGradidoTransaction()->getFingerprint(), transactionEntry });
			role->runPastAddToBlockchain(confirmedTransaction, blockchain);
			return true;
		}

		void InMemory::addTransactionTriggerEvent(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent)
		{
			std::lock_guard _lock(mTransactionTriggerEventsMutex);
			mTransactionTriggerEvents.insert({ transactionTriggerEvent->getTargetDate(), transactionTriggerEvent });
		}

		void InMemory::removeTransactionTriggerEvent(const data::TransactionTriggerEvent& transactionTriggerEvent)
		{
			std::lock_guard _lock(mTransactionTriggerEventsMutex);
			auto range = mTransactionTriggerEvents.equal_range(transactionTriggerEvent.getTargetDate());
			int countRemoved = 0;
			for (auto it = range.first; it != range.second;) {
				if (transactionTriggerEvent.isTheSame(it->second)) {
					it = mTransactionTriggerEvents.erase(it);
					countRemoved++;
				} else {
					it++;
				}
			}
			if (!countRemoved) {
				LOG_F(WARNING, "couldn't find transactionTriggerEvent for removal for transaction: %lu", transactionTriggerEvent.getLinkedTransactionId());
			}
			else if (countRemoved > 1) {
				LOG_F(WARNING, "find more than one transactionTriggerEvent for removal for transaction: %lu", transactionTriggerEvent.getLinkedTransactionId());
			}
		}


		bool InMemory::isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const
		{
			std::lock_guard _lock(mWorkMutex);
			auto signature = gradidoTransaction->getFingerprint();
			auto range = mTransactionFingerprintTransactionEntry.equal_range(*signature);
			for (auto& it = range.first; it != range.second; ++it) {
				auto itGradidoTransaction = it->second->getConfirmedTransaction()->getGradidoTransaction();
				if (gradidoTransaction->isTheSame(*itGradidoTransaction)) {
					return true;
				}
			}
			return false;
		}

		//! return events in asc order of targetDate
		std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> InMemory::findTransactionTriggerEventsInRange(TimepointInterval range)
		{
			std::lock_guard _lock(mTransactionTriggerEventsMutex);
			auto startIt = mTransactionTriggerEvents.lower_bound(range.getStartDate());
			auto endIt = mTransactionTriggerEvents.upper_bound(range.getEndDate());
			std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> result;
			result.reserve(std::distance(startIt, endIt));
			for (; startIt != endIt; startIt++) {
				result.push_back(startIt->second);
			}
			return result;
		}

		std::shared_ptr<const data::TransactionTriggerEvent> InMemory::findNextTransactionTriggerEventInRange(TimepointInterval range)
		{
			std::lock_guard _lock(mTransactionTriggerEventsMutex);
			auto startIt = mTransactionTriggerEvents.lower_bound(range.getStartDate());
			if (startIt != mTransactionTriggerEvents.end() && startIt->first <= range.getEndDate()) {
				return startIt->second;
			}
			return nullptr;
		}

		const TransactionEntries& InMemory::getSortedTransactions()
		{
			if (mSortedDirty) {
				std::lock_guard _lock(mWorkMutex);
				mSortedTransactions.clear();
				for (auto transactionNrEntryPair : mTransactionsByNr) {
					mSortedTransactions.push_back(transactionNrEntryPair.second);
				}
				mSortedDirty = false;
			}
			return mSortedTransactions;
		}

		TransactionEntries InMemory::findAll(const Filter& filter/* = Filter::ALL_TRANSACTIONS*/) const
		{
			std::lock_guard _lock(mWorkMutex);
			TransactionEntries result;
			// if pagination is used, filterCopy contain count of still to find transactions
			Filter filterCopy(filter);
			auto transactionNrs = mTransactionsIndex.findTransactions(filterCopy, mPublicKeyDirectory);
			for (auto transactionNr : transactionNrs) {
				if (!filter.pagination.hasCapacityLeft(result.size())) {
					break;
				}
				auto transaction = getTransactionForId(transactionNr);
				auto filterResult = filter.matches(transaction, FilterCriteria::FILTER_FUNCTION);
				if ((filterResult & FilterResult::USE) == FilterResult::USE) {
					result.push_back(transaction);
				}
				if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
					break;
				}
			}
			return result;
		}

		ConstTransactionEntryPtr InMemory::findOne(const Filter& filter/* = Filter::LAST_TRANSACTION*/) const
		{
			if (filter.isTheSame(Filter::LAST_TRANSACTION)) {
				return mLastTransaction;
			}
			return Abstract::findOne(filter);
		}

		data::AddressType InMemory::getAddressType(const Filter& filter/* = Filter::ALL_TRANSACTIONS*/) const
		{
			if (!filter.involvedPublicKey) {
				throw GradidoNodeInvalidDataException("missing public key, please use filter with involvedPublicKey set");
			}
			return mTransactionsIndex.getAddressType(filter.involvedPublicKey, mPublicKeyDirectory);
		}

		
		ConstTransactionEntryPtr InMemory::getTransactionForId(uint64_t transactionId) const
		{
			lock_guard _lock(mWorkMutex);
			auto it = mTransactionsByNr.find(transactionId);
			if (it != mTransactionsByNr.end()) {
				return it->second;
			}
			return nullptr;
		}

		ConstTransactionEntryPtr InMemory::findByLedgerAnchor(
			const data::LedgerAnchor& ledgerAnchor,
			const Filter& filter/* = Filter::ALL_TRANSACTIONS*/
		) const
		{
			std::lock_guard _lock(mWorkMutex);
			
			auto it = mLedgerAnchorTransactionNrs.find(ledgerAnchor);
			if (it != mLedgerAnchorTransactionNrs.end()) {
				return getTransactionForId(it->second);
			}
			return nullptr;
		}

		AbstractProvider* InMemory::getProvider() const
		{
			return InMemoryProvider::getInstance();
		}

		uint32_t InMemory::getOrAddPublicKey(const PublicKey& publicKey)
		{
			return mPublicKeyDirectory.getOrAddIndexForData(publicKey);
		}

		void InMemory::pushTransactionEntry(ConstTransactionEntryPtr transactionEntry)
		{
			std::lock_guard _lock(mWorkMutex);
			mSortedDirty = true;
			auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
			mTransactionsIndex.addIndicesForTransaction(transactionEntry, mPublicKeyDirectory);
			mLedgerAnchorTransactionNrs.insert({ confirmedTransaction->getLedgerAnchor(), confirmedTransaction->getId() });
			mTransactionsByNr.insert({ confirmedTransaction->getId(), transactionEntry });
			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
			mLastTransaction = transactionEntry;
		}

		void InMemory::removeTransactionEntry(ConstTransactionEntryPtr transactionEntry)
		{
			std::lock_guard _lock(mWorkMutex);
			mSortedDirty = true;
			auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
			mLedgerAnchorTransactionNrs.erase(confirmedTransaction->getLedgerAnchor());			
			mTransactionsByNr.erase(confirmedTransaction->getId());
		}
	}
}