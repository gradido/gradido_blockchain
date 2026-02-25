#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/InMemory.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
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
#include "gradido_protobuf_zig.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <memory>
#include <string_view>

using namespace magic_enum;
using std::string_view;
using std::lock_guard;
using std::make_shared;
using memory::ConstBlockPtr, memory::Block;

namespace gradido {

	using data::adapter::toPublicKey;
	using data::compact::ConfirmedGradidoTx, data::compact::ConfirmedTxs, data::compact::ConstConfirmedTxPtr;
	using data::AddressType, data::ConstGradidoTransactionPtr, data::Timestamp, data::LedgerAnchor, data::AccountBalance;
	using namespace interaction;

	namespace blockchain {
		InMemory::InMemory(string_view uniqueCommunityAlias, uint32_t communityIdIndex)
			: Abstract(communityIdIndex),
			mPublicKeyDirectory(std::string(uniqueCommunityAlias) + std::string("_publicKeyDictionary")),
			mTransactionsIndex(communityIdIndex), mSortedDirty(false), mExitCalled(false)
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

		bool InMemory::createAndAddConfirmedTransactionExternFast(
			data::ConstGradidoTransactionPtr gradidoTransaction,
			const data::LedgerAnchor& ledgerAnchor,
			std::vector<data::AccountBalance> accountBalances
		)
		{
			auto blockchain = getProvider()->findBlockchain(mCommunityIdIndex);
			confirmTransaction::Context context(blockchain);
			context.disableVerify();
			// context.disableRunningHashTest();
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
			auto transactionEntry = getTransactionForId(transactionTriggerEvent->getLinkedTransactionId());
			if (!transactionEntry) {
				throw GradidoNullPointerException(
					"empty transactionEntry from getTransactionForId for transactionTriggerEvent linked transaction id",
					"data::TransactionEntry",
					__FUNCTION__
				);
			}
			if (!transactionEntry->isDeferredTransfer()) {
				throw GradidoNodeInvalidDataException("transactionTriggerEvent linked transaction id is not a deferred transfer");
			}
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
				}
				else {
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


		/*bool InMemory::isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const
		{
			std::lock_guard _lock(mWorkMutex);
			Filter f;
			auto createdAt = gradidoTransaction->getTransactionBody()->getCreatedAt();
			f.timepointInterval = gradidoTransaction->getTransactionBody()->getCreatedAt();
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
		*/
		//! return events in asc order of targetDate
		std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> InMemory::findTransactionTriggerEventsInRange(Timestamp startDate, Timestamp endDate)
		{
			std::lock_guard _lock(mTransactionTriggerEventsMutex);
			auto startIt = mTransactionTriggerEvents.lower_bound(startDate);
			auto endIt = mTransactionTriggerEvents.upper_bound(endDate);
			std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> result;
			result.reserve(std::distance(startIt, endIt));
			for (; startIt != endIt; startIt++) {
				result.push_back(startIt->second);
			}
			return result;
		}

		std::shared_ptr<const data::TransactionTriggerEvent> InMemory::findNextTransactionTriggerEventInRange(Timestamp startDate, Timestamp endDate)
		{
			std::lock_guard _lock(mTransactionTriggerEventsMutex);
			auto startIt = mTransactionTriggerEvents.lower_bound(startDate);
			if (startIt != mTransactionTriggerEvents.end() && startIt->first <= endDate) {
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
			auto transactionNrs = mTransactionsIndex.findTransactions(filterCopy, mPublicKeyDirectory, mCommunityIdIndex);
			for (auto transactionNr : transactionNrs) {
				if (!filter.pagination.hasCapacityLeft(result.size())) {
					break;
				}
				auto transaction = getTransactionForId(transactionNr);
				auto filterResult = filter.matches(transaction, FilterCriteria::FILTER_FUNCTION | FilterCriteria::TIMEPOINT_INTERVAL);
				if ((filterResult & FilterResult::USE) == FilterResult::USE) {
					result.push_back(transaction);
				}
				if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
					break;
				}
			}
			return result;
		}

		ConfirmedTxs InMemory::findAll(const CompactFilter& originalFilter) const
		{
			std::lock_guard _lock(mWorkMutex);
			CompactFilter filter(originalFilter);
			filter.pagination.page = 1;
			ConfirmedTxs results;
			int paginationCursor = 0;
			// for search for transactions changing balance of publicKey only (register address and community root setting balance to zero will also be returned)
			if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType && filter.publicKeyIndex.communityIdIndex == mCommunityIdIndex) {
				auto skipEntries = originalFilter.pagination.skipEntriesCount();
				do {
					auto balanceChangingTxsInRange = mTransactionsIndex.findTransactionsBalanceChangingForPublicKey(filter);
					if (balanceChangingTxsInRange.empty()) {
						break;
					}
					for (const auto& tx : balanceChangingTxsInRange) {
						auto transaction = getConfirmedTxForId(tx);
						if (!transaction) { 
							throw GradidoBlockchainTransactionNotFoundException("confirmed tx not found").setTransactionId(tx);
						}
						auto filterResult = filter.matches(*transaction, FilterCriteria::TIMEPOINT_INTERVAL);
						if ((filterResult & FilterResult::USE) == FilterResult::USE) {
							if (paginationCursor >= skipEntries) {
								results.push_back(transaction);
								if (!originalFilter.pagination.hasCapacityLeft(results.size())) {
									return results;
								}
							}
							paginationCursor++;
						}
						if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
							return results;
						}
					}
					if (filter.pagination.empty() || filter.pagination.size > balanceChangingTxsInRange.size()) {
						break;
					}
					filter.pagination.page++;
				} while (originalFilter.pagination.hasCapacityLeft(results.size()));
				return results;
			}
			// search iterator based, intern sorted by confirmedAt date year and month, optimized for search for transactions from a specific time intervall
			auto startIt = mTransactionsIndex.begin(filter);
			auto endIt = mTransactionsIndex.end(filter);
			int cursor = 0;
			auto it = startIt;
			for (; it != endIt; ++it) 
			{	
				auto transaction = getConfirmedTxForId(*it);
				if (!transaction) {
					throw GradidoBlockchainTransactionNotFoundException("confirmed tx not found").setTransactionId(*it);
				}
				auto filterResult = filter.matches(*transaction, FilterCriteria::TIMEPOINT_INTERVAL);
				if ((filterResult & FilterResult::USE) == FilterResult::USE) {
					if (paginationCursor >= filter.pagination.skipEntriesCount()) {
						results.push_back(transaction);
						if (!filter.pagination.hasCapacityLeft(results.size())) {
							break;
						}
					}
					paginationCursor++;
				}
				if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
					break;
				}
				++cursor;
			}
			return results;
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
			if (!filter.involvedPublicKey || filter.involvedPublicKey->isEmpty()) {
				throw GradidoNodeInvalidDataException("missing public key, please use filter with involvedPublicKey set");
			}
			auto publicKeyIndexOptional = mPublicKeyDirectory.getIndexForData(toPublicKey(filter.involvedPublicKey));
			if (!publicKeyIndexOptional) {
				return AddressType::NONE;
			}
			uint32_t publicKeyUint32 = (uint32_t)publicKeyIndexOptional;
			if (publicKeyUint32 != publicKeyIndexOptional) {
				throw GradidoNodeInvalidDataException("public key index overflow");
			}
			auto addressTypeStateChange = mTransactionsIndex.getAddressType({ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = publicKeyUint32 });
			if (addressTypeStateChange.getTxId()) {
				auto tx = getTransactionForId(addressTypeStateChange.getTxId());
				if (!tx) {
					throw GradidoNodeInvalidDataException("tx for tx id in address index couldn't be found");
				}
				if (FilterResult::USE != (FilterResult::USE & filter.matches(tx, FilterCriteria::MAX))) {
					return getAddressTypeSlow(filter);
				}
			}
			return addressTypeStateChange.getValue();
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

		data::compact::ConstConfirmedTxPtr InMemory::getConfirmedTxForId(uint64_t transactionId) const
		{
			lock_guard _lock(mWorkMutex);
			auto it = mConfirmedTxByNr.find(transactionId);
			if (it != mConfirmedTxByNr.end()) {
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
			// create compact version
			try {
				uint8_t buffer[1024];
				grdu_memory alloc;
				grdu_memory_init_static(&alloc, buffer, 1024);
				grdw_confirmed_transaction tx{};
				transactionEntry->getConfirmedTransaction()->toGrdw(&alloc, &tx, mCommunityIdIndex);
				auto confirmedTx =  make_shared<ConfirmedGradidoTx>(ConfirmedGradidoTx::fromGrdw(&tx, mCommunityIdIndex, *g_appContext));
				alloc.last_index = 0;
				grdw_transaction_body txBody{};
				transactionEntry->getTransactionBody()->toGrdw(&alloc, &txBody);
				confirmedTx->fillFromGrdwTransactionBody(&txBody, *g_appContext);
				mConfirmedTxByNr.insert({ confirmedTx->txNr, std::move(confirmedTx) });
			}
			catch (GradidoBlockchainException& ex) {
				LOG_F(WARNING, "%s on create compact", ex.getFullString().c_str());
			}
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