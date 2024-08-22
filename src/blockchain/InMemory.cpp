#include "gradido_blockchain/blockchain/InMemory.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Type.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"

#include <algorithm>

namespace gradido {
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

		bool InMemory::addGradidoTransaction(data::ConstGradidoTransactionPtr gradidoTransaction, memory::ConstBlockPtr messageId, Timepoint confirmedAt)
		{
			auto provider = getProvider();
			interaction::validate::Context validateGradidoTransaction(*gradidoTransaction);
			validateGradidoTransaction.run(interaction::validate::Type::SINGLE, mCommunityId, provider);
			std::lock_guard _lock(mWorkMutex);
			if (mExitCalled) return false;
			if (isTransactionExist(gradidoTransaction)) {
				return false;
			}
			uint64_t id = 1;
			auto lastTransaction = findOne(Filter::LAST_TRANSACTION);
			if (lastTransaction) {
				id = lastTransaction->getTransactionNr() + 1;
			}
			else {
				mStartDate = gradidoTransaction->getTransactionBody()->getCreatedAt();
			}
				
			auto serializedTransaction = gradidoTransaction->getSerializedTransaction();
			auto body = gradidoTransaction->getTransactionBody();
			if (!messageId) {
				// fake message id simply with taking hash from serialized transaction,
				// iota message id will normally calculated with same algorithmus but with additional data 
				messageId = std::make_shared<memory::Block>(serializedTransaction->calculateHash());
			}

			interaction::calculateAccountBalance::Context finalBalanceCalculate(*this);
			auto finalBalance = finalBalanceCalculate.run(gradidoTransaction, confirmedAt, id);

			auto confirmedTransaction = std::make_shared<data::ConfirmedTransaction>(
				id,
				gradidoTransaction,
				confirmedAt,
				GRADIDO_CONFIRMED_TRANSACTION_V3_3_VERSION_STRING,
				nullptr,
				messageId,
				finalBalance
			);
			data::ConstConfirmedTransactionPtr lastConfirmedTransaction;
			if (lastTransaction) {
				lastConfirmedTransaction = lastTransaction->getConfirmedTransaction();
				if (confirmedAt < lastConfirmedTransaction->getConfirmedAt().getAsTimepoint()) {
					throw BlockchainOrderException("previous transaction is younger");
				}
			}
			confirmedTransaction->getRunningHash() = std::make_shared<memory::Block>(confirmedTransaction->calculateRunningHash(lastConfirmedTransaction));
			// important! validation
			interaction::validate::Context validate(*confirmedTransaction);
			interaction::validate::Type level =
				// interaction::validate::Type::SINGLE | // already checked
				interaction::validate::Type::PREVIOUS |
				interaction::validate::Type::MONTH_RANGE |
				interaction::validate::Type::ACCOUNT
			;
			if (body->getType() != data::CrossGroupType::LOCAL) {
				level = level | interaction::validate::Type::PAIRED;
			}
			// throw if some error occure
			validate.run(level, getCommunityId(), provider);

			auto transactionEntry = std::make_shared<TransactionEntry>(confirmedTransaction);
			pushTransactionEntry(transactionEntry);
			mTransactionFingerprintTransactionEntry.insert({ confirmedTransaction->getGradidoTransaction()->getFingerprint(), transactionEntry});

			return true;
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
			auto processEntry = [&](auto& start, auto& end, FilterCriteria toFilter, const Filter& filter) -> TransactionEntries 
			{
				end--;
				TransactionEntries transactionEntries;
				bool revert = SearchDirection::DESC == filter.searchDirection;
				auto& it = revert ? end : start;
				int paginationCursor = 0;
				while (it != (revert ? start : end)) {
					auto result = filter.matches(it->second, toFilter, mCommunityId);
					if ((result & FilterResult::USE) == FilterResult::USE) {
						if (paginationCursor >= filter.pagination.skipEntriesCount()) {
							transactionEntries.push_back(it->second);
							if (filter.pagination.size && transactionEntries.size() >= filter.pagination.size) {
								return transactionEntries;
							}
						}
						paginationCursor++;
					}
					if ((result & FilterResult::STOP) == FilterResult::STOP) {
						return transactionEntries;
					}
					if (!revert) {
						++it;
					}
					else {
						--it;
					}
				}
				// last entry
				auto result = filter.matches(it->second, toFilter, mCommunityId);
				if ((result & FilterResult::USE) == FilterResult::USE) {
					transactionEntries.push_back(start->second);
				}
				return transactionEntries;
			};

			if ((startSetType & FilterCriteria::TIMEPOINT_INTERVAL) == FilterCriteria::TIMEPOINT_INTERVAL)
			{
				auto notYetFiltered = FilterCriteria::MAX - FilterCriteria::TIMEPOINT_INTERVAL;
				data::TimestampSeconds startDate(filter.timepointInterval.getStartDate());
				data::TimestampSeconds endDate(filter.timepointInterval.getEndDate());
				auto startIt = mTransactionsByConfirmedAt.lower_bound(startDate);
				auto endIt = mTransactionsByConfirmedAt.upper_bound(endDate);
				return processEntry(startIt, endIt, notYetFiltered, filter);
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
				auto prefilteredTransactions = processEntry(range.first, range.second, notYetFiltered, partFilter);
				// and if a filter function exist we sort and call it in correct order
				if (filter.filterFunction && !prefilteredTransactions.empty()) {
					std::map<uint64_t, std::shared_ptr<TransactionEntry>> sortedTransactions;
					for (std::shared_ptr<TransactionEntry> entry : prefilteredTransactions) {
						sortedTransactions.insert({ entry->getTransactionNr(), entry });
					}
					prefilteredTransactions.clear();
					auto startIt = sortedTransactions.begin();
					auto endIt = sortedTransactions.end();
					return processEntry(startIt, endIt, FilterCriteria::FILTER_FUNCTION, filter);
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
				return processEntry(startIt, endIt, notYetFiltered, filter);
			}
			throw std::runtime_error("not expected branch");			
		}

		//! find all deferred transfers which have the timeout in date range between start and end, have senderPublicKey and are not redeemed,
		//! therefore boocked back to sender
		TransactionEntries InMemory::findTimeoutedDeferredTransfersInRange(
			memory::ConstBlockPtr senderPublicKey,
			TimepointInterval timepointInterval,
			uint64_t maxTransactionNr
		) const
		{
			TransactionEntries result;
			auto it = mTimeoutDeferredRedeemedTransferPairs.lower_bound(timepointInterval.getStartDate());
			while(it != mTimeoutDeferredRedeemedTransferPairs.upper_bound(timepointInterval.getEndDate())) 
			{
				auto deferredRedeemedPair = it->second;
				assert(deferredRedeemedPair.first->isDeferredTransfer());
				// not redeemed
				if (!deferredRedeemedPair.second) {
					auto deferredTransfer = deferredRedeemedPair.first->getTransactionBody()->getDeferredTransfer();
					if (deferredTransfer->getSenderPublicKey()->isTheSame(senderPublicKey)) {
						result.push_back(deferredRedeemedPair.first);
					}
				}
				++it;
			}
			return result;
		}

		//! find all transfers which redeem a deferred transfer in date range (timepointInterval)
		//! \param senderPublicKey sender public key of sending account of deferred transaction
		//! \return list with transaction pairs, first is deferred transfer, second is redeeming transfer
		std::list<DeferredRedeemedTransferPair> InMemory::findRedeemedDeferredTransfersInRange(
			memory::ConstBlockPtr senderPublicKey,
			TimepointInterval timepointInterval,
			uint64_t maxTransactionNr
		) const
		{
			std::list<DeferredRedeemedTransferPair> result;
			// go back in time, deferred transfers older then GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL cannot be redeemed in timepointInterval
			auto startDate = timepointInterval.getStartDate() - GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL;
			auto it = mTimeoutDeferredRedeemedTransferPairs.lower_bound(startDate);
			while (it != mTimeoutDeferredRedeemedTransferPairs.upper_bound(timepointInterval.getEndDate())) {
				auto deferredRedeemedPair = it->second;
				assert(deferredRedeemedPair.first->isDeferredTransfer());
				// redeemed
				if (deferredRedeemedPair.second) {
					auto deferredTransfer = deferredRedeemedPair.first->getTransactionBody()->getDeferredTransfer();
					if (deferredTransfer->getSenderPublicKey()->isTheSame(senderPublicKey)) {
						result.push_back(deferredRedeemedPair);
					}
				}
				++it;
			}

			return result;
		}

		std::shared_ptr<TransactionEntry> InMemory::getTransactionForId(uint64_t transactionId) const
		{
			std::lock_guard _lock(mWorkMutex);
			auto it = mTransactionsByNr.find(transactionId);
			if (it != mTransactionsByNr.end()) {
				return it->second;
			}
			return nullptr;
		}

		std::shared_ptr<TransactionEntry> InMemory::findByMessageId(
			memory::ConstBlockPtr messageId,
			const Filter& filter/* = Filter::ALL_TRANSACTIONS*/
		) const
		{
			std::lock_guard _lock(mWorkMutex);
			auto it = mMessageIdTransactionNrs.find(iota::MessageId::fromMemoryBlock(*messageId));
			if (it != mMessageIdTransactionNrs.end()) {
				return getTransactionForId(it->second);
			}
			return nullptr;
		}

		AbstractProvider* InMemory::getProvider() const
		{
			return InMemoryProvider::getInstance();
		}

		void InMemory::pushTransactionEntry(std::shared_ptr<TransactionEntry> transactionEntry)
		{
			std::lock_guard _lock(mWorkMutex);
			mSortedDirty = true;
			auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
			auto involvedAddresses = confirmedTransaction->getGradidoTransaction()->getInvolvedAddresses();

			mTransactionsByConfirmedAt.insert({ confirmedTransaction->getConfirmedAt(), transactionEntry});
			for (auto involvedAddress : involvedAddresses) {
				mTransactionsByPubkey.insert({ involvedAddress, transactionEntry });
			}
			mMessageIdTransactionNrs.insert({
				iota::MessageId::fromMemoryBlock(*confirmedTransaction->getMessageId()),
				confirmedTransaction->getId()
				});
			mTransactionsByNr.insert({
				confirmedTransaction->getId(),
				transactionEntry
				});
			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
			// add deferred transfer to special deferred transfer map
			if (body->isDeferredTransfer()) {
				mTimeoutDeferredRedeemedTransferPairs.insert({ body->getDeferredTransfer()->getTimeout().getAsTimepoint(), {transactionEntry, nullptr}});
			}
			// find out if transaction redeem a deferred transfer
			if (body->isTransfer()) {
				FilterBuilder filterBuilder;
				
				auto lastFromSameSender = findOne(
					filterBuilder
					.setInvolvedPublicKey(body->getTransfer()->getSender().getPubkey())
					.setMaxTransactionNr(confirmedTransaction->getId() - 1)
					.setSearchDirection(SearchDirection::DESC)
					.setPagination(Pagination(1))
					.build()
				);
				if (lastFromSameSender) {
					auto lastFromSameSenderBody = lastFromSameSender->getTransactionBody();
					auto lastFromSameSenderRecipient = lastFromSameSenderBody->getDeferredTransfer()->getTransfer().getRecipient();
					auto pubkey = body->getTransfer()->getSender().getPubkey();
					if (lastFromSameSenderBody->isDeferredTransfer() &&
						lastFromSameSenderRecipient->isTheSame(pubkey)) {
						auto lastFromSameSenderTimeout = lastFromSameSenderBody->getDeferredTransfer()->getTimeout();
						// seems we found a matching deferred transfer transaction
						auto byTimeoutDeferredRedeemedPairsIt = mTimeoutDeferredRedeemedTransferPairs.equal_range(lastFromSameSenderTimeout.getAsTimepoint());
						for (auto it = byTimeoutDeferredRedeemedPairsIt.first; it != byTimeoutDeferredRedeemedPairsIt.second; ++it) {
							if (lastFromSameSender->getSerializedTransaction()->isTheSame(it->second.first->getSerializedTransaction())) {
								it->second.second = transactionEntry;
								break;
							}
						}
					}
				}
			}
		}

		void InMemory::removeTransactionEntry(std::shared_ptr<TransactionEntry> transactionEntry)
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
			mMessageIdTransactionNrs.erase(iota::MessageId::fromMemoryBlock(*confirmedTransaction->getMessageId()));
			mTransactionsByNr.erase(confirmedTransaction->getId());

			auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
			// remove deferred transfer from special deferred transfer map
			if (body->isDeferredTransfer()) {
				auto byTimeoutDeferredRedeemedPairsIt = mTimeoutDeferredRedeemedTransferPairs.equal_range(
					body->getDeferredTransfer()->getTimeout().getAsTimepoint()
				);
				for (auto it = byTimeoutDeferredRedeemedPairsIt.first; it != byTimeoutDeferredRedeemedPairsIt.second; ++it) {
					if (serializedTransaction->isTheSame(it->second.first->getSerializedTransaction())) {
						mTimeoutDeferredRedeemedTransferPairs.erase(it);
						break;
					}
				}
			}
		}

		FilterCriteria InMemory::findSmallestPrefilteredTransactionList(const Filter& filter) const
		{
			std::map<ptrdiff_t, FilterCriteria> resultCounts;
			if (!filter.timepointInterval.isEmpty()) {
				data::TimestampSeconds startDate(filter.timepointInterval.getStartDate());
				data::TimestampSeconds endDate(filter.timepointInterval.getEndDate());
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
			auto startIt = mTransactionsByNr.lower_bound(filter.minTransactionNr);
			auto endIt = mTransactionsByNr.end();
			if (filter.maxTransactionNr) {
				endIt = mTransactionsByNr.upper_bound(filter.maxTransactionNr);
			}
			if (startIt != mTransactionsByNr.end() && startIt != endIt) {
				resultCounts.emplace(std::distance(startIt, endIt), FilterCriteria::TRANSACTION_NR);
			}
			
			if (resultCounts.empty()) { return FilterCriteria::NONE; }

			// map is auto sorted and begin is smallest distance between iterators
			return resultCounts.begin()->second;
		}

		bool InMemory::isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const
		{
			std::lock_guard _lock(mWorkMutex);
			auto signature = gradidoTransaction->getFingerprint();
			auto range = mTransactionFingerprintTransactionEntry.equal_range(signature);
			for (auto it = range.first; it != range.second; ++it) {
				if (it->second->getConfirmedTransaction()->getGradidoTransaction()->getFingerprint()->isTheSame(signature)) {
					return true;
				}
			}	
			return false;
		}
	}
}