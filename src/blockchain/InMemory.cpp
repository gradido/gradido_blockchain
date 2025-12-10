#include "gradido_blockchain/blockchain/InMemory.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
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

using namespace magic_enum;

namespace gradido {
	using namespace data;
	using namespace interaction;
	namespace blockchain {
		InMemory::InMemory(std::string_view communityId)
			: Abstract(communityId), mSortedDirty(false), mExitCalled(false)
		{

		}

		InMemory::~InMemory()
		{
		}

		void InMemory::clear()
		{
			std::lock_guard _lock(mWorkMutex);
			mTransactionsByPubkey.clear();
			mTransactionsByConfirmedAt.clear();
			mSortedDirty = false;
			mSortedTransactions.clear();
		}

		void InMemory::exit()
		{
			std::lock_guard _lock(mWorkMutex);
			mExitCalled = true;
		}

		bool InMemory::createAndAddConfirmedTransaction(
			ConstGradidoTransactionPtr gradidoTransaction,
			memory::ConstBlockPtr messageId,
			Timestamp confirmedAt
		) {
			auto blockchain = getProvider()->findBlockchain(mCommunityId);
			confirmTransaction::Context context(blockchain);
			auto role = context.createRole(gradidoTransaction, messageId, confirmedAt);
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
			auto transactionEntry = std::make_shared<TransactionEntry>(confirmedTransaction);
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
			for (auto& it = range.first; it != range.second; it++) {
				if (transactionTriggerEvent.isTheSame(it->second)) {
					mTransactionTriggerEvents.erase(it);
					return;
				}
			}
			LOG_F(WARNING, "couldn't find transactionTriggerEvent for removal for transaction: %llu", transactionTriggerEvent.getLinkedTransactionId());
		}


		bool InMemory::isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const
		{
			std::lock_guard _lock(mWorkMutex);
			auto signature = gradidoTransaction->getFingerprint();
			auto range = mTransactionFingerprintTransactionEntry.equal_range(*signature);
			for (auto& it = range.first; it != range.second; ++it) {
				auto itGradidoTransaction = it->second->getConfirmedTransaction()->getGradidoTransaction();
				if (
						itGradidoTransaction->getFingerprint()->isTheSame(signature) &&
						itGradidoTransaction->getBodyBytes()->isTheSame(gradidoTransaction->getBodyBytes())
				) {
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
			// find smallest start set
			auto startSetType = findSmallestPrefilteredTransactionList(filter);
			if (FilterCriteria::NONE == startSetType) {
				return {};
			}

			if ((startSetType & FilterCriteria::TIMEPOINT_INTERVAL) == FilterCriteria::TIMEPOINT_INTERVAL)
			{
				auto notYetFiltered = FilterCriteria::MAX - FilterCriteria::TIMEPOINT_INTERVAL;
				const auto& timeInterval = filter.timepointInterval;
				// first element not less than the given key
				auto startIt = mTransactionsByConfirmedAt.lower_bound(timeInterval.getStartDate());
				// first element greater than the given key (outside of bounds, possible == end())
				auto endIt = mTransactionsByConfirmedAt.upper_bound(timeInterval.getEndDate());

				return findInRange(startIt, endIt, notYetFiltered, filter);
			}						
			else if ((startSetType & FilterCriteria::INVOLVED_PUBLIC_KEY) == FilterCriteria::INVOLVED_PUBLIC_KEY) {
				// we have a problem there, filterFunction expect to be called in searchOrder, mTransactionsByPubkey is sorted by public key
				// so we first filter without filterFunction to reduce result set as much as possible
				auto notYetFiltered = FilterCriteria::MAX - FilterCriteria::FILTER_FUNCTION;
				memory::BlockKey key(filter.involvedPublicKey);
				auto range = mTransactionsByPubkey.equal_range(filter.involvedPublicKey);
				Filter partFilter = filter;
				// disable pagination for prefilter round
				partFilter.pagination = Pagination();		
				partFilter.searchDirection = SearchDirection::ASC;
				auto prefilteredTransactions = findInRange(range.first, range.second, notYetFiltered, partFilter);

				// we need to call processEntry again for filterFunction, searchDirection and/or pagination
				if (!prefilteredTransactions.empty()) {
					std::map<uint64_t, std::shared_ptr<const TransactionEntry>> sortedTransactions;
					for (std::shared_ptr<const TransactionEntry> entry : prefilteredTransactions) {
						sortedTransactions.insert({ entry->getTransactionNr(), entry });
					}
					prefilteredTransactions.clear();
					auto startIt = sortedTransactions.begin();
					auto endIt = sortedTransactions.end();
					return findInRange(startIt, endIt, FilterCriteria::FILTER_FUNCTION, filter);
				}
				else {
					return prefilteredTransactions;
				}
			}
			else {
				auto notYetFiltered = FilterCriteria::MAX - FilterCriteria::TRANSACTION_NR;
				auto startIt = mTransactionsByNr.lower_bound(filter.minTransactionNr);
				if (mTransactionsByNr.end() == startIt) {
					return {};
				}
				auto endIt = mTransactionsByNr.end();
				if (filter.maxTransactionNr) {
					endIt = mTransactionsByNr.upper_bound(filter.maxTransactionNr);
				}
				return findInRange(startIt, endIt, notYetFiltered, filter);
			}
			throw std::runtime_error("not expected branch");			
		}

		
		std::shared_ptr<const TransactionEntry> InMemory::getTransactionForId(uint64_t transactionId) const
		{
			std::lock_guard _lock(mWorkMutex);
			auto it = mTransactionsByNr.find(transactionId);
			if (it != mTransactionsByNr.end()) {
				return it->second;
			}
			return nullptr;
		}

		std::shared_ptr<const TransactionEntry> InMemory::findByMessageId(
			memory::ConstBlockPtr messageId,
			const Filter& filter/* = Filter::ALL_TRANSACTIONS*/
		) const
		{
			deserialize::Context transactionIdDeserialize(messageId, deserialize::Type::HIERO_TRANSACTION_ID);
			transactionIdDeserialize.run();
			if (!transactionIdDeserialize.isHieroTransactionId()) {
				return nullptr;
			}
			std::lock_guard _lock(mWorkMutex);
			
			auto it = mHieroTransactionIdTransactionNrs.find(transactionIdDeserialize.getHieroTransactionId());
			if (it != mHieroTransactionIdTransactionNrs.end()) {
				return getTransactionForId(it->second);
			}
			return nullptr;
		}

		AbstractProvider* InMemory::getProvider() const
		{
			return InMemoryProvider::getInstance();
		}

		void InMemory::pushTransactionEntry(std::shared_ptr<const TransactionEntry> transactionEntry)
		{
			std::lock_guard _lock(mWorkMutex);
			mSortedDirty = true;
			auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
			auto involvedAddresses = confirmedTransaction->getInvolvedAddresses();

			mTransactionsByConfirmedAt.insert({ confirmedTransaction->getConfirmedAt(), transactionEntry});
			for (auto involvedAddress : involvedAddresses) {
				mTransactionsByPubkey.insert({ involvedAddress, transactionEntry });
			}
			if (confirmedTransaction->getMessageId()) {
				deserialize::Context transactionIdDeserialize(confirmedTransaction->getMessageId(), deserialize::Type::HIERO_TRANSACTION_ID);
				transactionIdDeserialize.run();
				if (!transactionIdDeserialize.isHieroTransactionId()) {
					throw GradidoNodeInvalidDataException("invalid transaction id on transactionEntry");
				}
				mHieroTransactionIdTransactionNrs.insert({
					transactionIdDeserialize.getHieroTransactionId(),
					confirmedTransaction->getId()
					});
			}
			mTransactionsByNr.insert({
				confirmedTransaction->getId(),
				transactionEntry
				});
			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
		}

		void InMemory::removeTransactionEntry(std::shared_ptr<const TransactionEntry> transactionEntry)
		{
			std::lock_guard _lock(mWorkMutex);
			mSortedDirty = true;
			auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
			auto byReceivedIt = mTransactionsByConfirmedAt.equal_range(confirmedTransaction->getConfirmedAt());
			auto serializedTransaction = transactionEntry->getSerializedTransaction();
			for (auto it = byReceivedIt.first; it != byReceivedIt.second; ++it)
			{
				if (serializedTransaction->isTheSame(it->second->getSerializedTransaction()))
				{
					it = mTransactionsByConfirmedAt.erase(it);
					if (it == byReceivedIt.second) {
						break;
					}
				}
			}
			auto involvedAddresses = confirmedTransaction->getGradidoTransaction()->getInvolvedAddresses();
			for (auto involvedAddress : involvedAddresses)
			{
				auto byPublicKeyIt = mTransactionsByPubkey.equal_range(involvedAddress);
				for (auto it = byPublicKeyIt.first; it != byPublicKeyIt.second; ++it)
				{
					if (serializedTransaction->isTheSame(it->second->getSerializedTransaction()))
					{
						it = mTransactionsByPubkey.erase(it);
						if (it == byPublicKeyIt.second) {
							break;
						}
					}
				}
			}
			if (confirmedTransaction->getMessageId()) {
				deserialize::Context transactionIdDeserialize(confirmedTransaction->getMessageId(), deserialize::Type::HIERO_TRANSACTION_ID);
				transactionIdDeserialize.run();
				if (transactionIdDeserialize.isHieroTransactionId()) {
					mHieroTransactionIdTransactionNrs.erase(transactionIdDeserialize.getHieroTransactionId());
				}
			}
			
			mTransactionsByNr.erase(confirmedTransaction->getId());
		}

		FilterCriteria InMemory::findSmallestPrefilteredTransactionList(const Filter& filter) const
		{
			std::map<ptrdiff_t, FilterCriteria> resultCounts;
			if (!filter.timepointInterval.isEmpty()) {
				data::Timestamp startDate(filter.timepointInterval.getStartDate());
				data::Timestamp endDate(filter.timepointInterval.getEndDate());
				auto startIt = mTransactionsByConfirmedAt.lower_bound(startDate);
				auto endIt = mTransactionsByConfirmedAt.upper_bound(endDate);
				if (startIt != mTransactionsByConfirmedAt.end() && startIt != endIt) {
					resultCounts.emplace(std::distance(startIt, endIt), FilterCriteria::TIMEPOINT_INTERVAL);
				}
			}	
			if (filter.involvedPublicKey && !filter.involvedPublicKey->isEmpty()) {
				auto its = mTransactionsByPubkey.equal_range(filter.involvedPublicKey);
				if (its.first != mTransactionsByPubkey.end() && its.first != its.second) {
					resultCounts.emplace(std::distance(its.first, its.second), FilterCriteria::INVOLVED_PUBLIC_KEY);
				}
			}
			if (!mTransactionsByNr.empty()) {
				assert(mTransactionsByNr.begin()->first == 1);
				auto endIt = mTransactionsByNr.end();
				endIt--;
				auto maxTransactionNr = endIt->first;
				if (filter.maxTransactionNr && filter.maxTransactionNr < maxTransactionNr) {
					maxTransactionNr = filter.maxTransactionNr;
				}
				resultCounts.emplace(maxTransactionNr - filter.minTransactionNr, FilterCriteria::TRANSACTION_NR);
			}
			
			if (resultCounts.empty()) { return FilterCriteria::NONE; }

			// map is auto sorted and begin is smallest distance between iterators
			return resultCounts.begin()->second;
		}

	}
}