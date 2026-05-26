#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain_core/types/transaction.h"

#include <optional>
#include <functional>
#include <memory>

using memory::ConstBlockPtr;
using std::optional, std::function;
using std::shared_ptr;

namespace gradido {
	namespace blockchain {

		Filter::Filter()
			: minTransactionNr(0),
			maxTransactionNr(0),
			searchDirection(SearchDirection::DESC),
			transactionType(GRDT_TRANSACTION_NONE),
			filterFunction(nullptr)			
		{
		}

		Filter::Filter(std::function<FilterResult(const TransactionEntry&)> _filterFunction)
			: minTransactionNr(0),
			maxTransactionNr(0),
			searchDirection(SearchDirection::DESC),
			transactionType(GRDT_TRANSACTION_NONE),
			filterFunction(_filterFunction)			
		{
		}

		Filter::Filter(
			uint64_t _minTransactionNr,
			uint64_t _maxTransactionNr,
			memory::ConstBlockPtr _involvedPublicKey /*= nullptr*/,
			SearchDirection _searchDirection /*= SearchDirection::DESC*/,
			Pagination _pagination /*= Pagination(0)*/,
			optional<uint32_t> _coinCommunityIdIndex /*= std::nullopt() */,
			TimepointInterval _timepointInterval/* = MonthYearInterval()*/,
			grdt_transaction _transactionType /* = data::GRDT_TRANSACTION_NONE*/,
			function<FilterResult(const TransactionEntry&)> _filterFunction/* = nullptr*/
		) :
			minTransactionNr(_minTransactionNr),
			maxTransactionNr(_maxTransactionNr),
			involvedPublicKey(_involvedPublicKey),
			searchDirection(_searchDirection),
			pagination(_pagination),
			coinCommunityIdIndex(_coinCommunityIdIndex),
			timepointInterval(_timepointInterval),
			transactionType(_transactionType),
			filterFunction(_filterFunction)
		{
		}

		// constructor for calculate creation sum in validate GradidoCreationRole
		Filter::Filter(
			uint64_t _maxTransactionNr,
			ConstBlockPtr _involvedPublicKey,
			TimepointInterval _timepointInterval,
			function<FilterResult(const TransactionEntry&)> _filterFunction
		) :
			minTransactionNr(0),
			maxTransactionNr(_maxTransactionNr),
			involvedPublicKey(_involvedPublicKey),
			searchDirection(SearchDirection::DESC),
			timepointInterval(_timepointInterval),
			transactionType(GRDT_TRANSACTION_NONE),
			filterFunction(_filterFunction)
		{
		}

		// constructor for calculate account balance
		Filter::Filter(
			uint64_t _maxTransactionNr,
			ConstBlockPtr _involvedPublicKey,
			SearchDirection _searchDirection,
			optional<uint32_t> _coinCommunityIdIndex,
			function<FilterResult(const TransactionEntry&)> _filterFunction
		) :
			minTransactionNr(0),
			maxTransactionNr(_maxTransactionNr),
			involvedPublicKey(_involvedPublicKey),
			searchDirection(_searchDirection),
			coinCommunityIdIndex(_coinCommunityIdIndex),
			transactionType(GRDT_TRANSACTION_NONE),
			filterFunction(_filterFunction)
		{
		}


		const Filter Filter::LAST_TRANSACTION = Filter(
			0, 0, nullptr, SearchDirection::DESC, Pagination(1, 0)
		);
		const Filter Filter::FIRST_TRANSACTION = Filter(
			0, 0, nullptr, SearchDirection::ASC, Pagination(1, 0)
		);
		const Filter Filter::ALL_TRANSACTIONS = Filter(
			0, 0, nullptr, SearchDirection::DESC, Pagination(0, 0)
		);

		Filter Filter::lastBalanceFor(memory::ConstBlockPtr updatedBalancePublicKey)
		{
			Filter f;
			f.updatedBalancePublicKey = updatedBalancePublicKey;
			f.pagination.size = 1;
			f.searchDirection = SearchDirection::DESC;
			return f;
		}

		FilterResult Filter::matches(shared_ptr<const TransactionEntry> entry, FilterCriteria type) const
		{
			// without needing deserialize transaction
			if ((type & FilterCriteria::TRANSACTION_NR) == FilterCriteria::TRANSACTION_NR) 
			{
				if (minTransactionNr && entry->getTransactionNr() < minTransactionNr) {
					return FilterResult::DISMISS;
				}
				if (maxTransactionNr && entry->getTransactionNr() > maxTransactionNr) {
					return FilterResult::DISMISS;
				}
			}
			if ((type & FilterCriteria::COIN_COMMUNITY) == FilterCriteria::COIN_COMMUNITY && coinCommunityIdIndex.has_value())
			{
				// if transaction hasn't explicit set coin community index, then it belongs to his blockchain
				auto entryCoinCommunityIdIndex = entry->getCoinCommunityIdIndex();
				if (!entryCoinCommunityIdIndex.has_value()) {
					entryCoinCommunityIdIndex = entry->getBlockchainCommunityIdIndex();
				}
				assert(entryCoinCommunityIdIndex.has_value());
				// only if coin community id was set transaction 
				if (coinCommunityIdIndex.value() != entryCoinCommunityIdIndex.value()) {
						return FilterResult::DISMISS;
				}					
			}
			if ((type & FilterCriteria::TRANSACTION_TYPE) == FilterCriteria::TRANSACTION_TYPE) {
				if (transactionType != GRDT_TRANSACTION_NONE) {
					if (entry->getTransactionType() != transactionType) {
						return FilterResult::DISMISS;
					}
				}
			}
			// has actually two versions without deserialize and with deserialize
			if ((type & FilterCriteria::TIMEPOINT_INTERVAL) == FilterCriteria::TIMEPOINT_INTERVAL) 
			{
				if (!timepointInterval.isEmpty()) {
					if (!timepointInterval.isInsideInterval(entry->getMonth(), entry->getYear())) {
						return FilterResult::DISMISS;
					}
					// change to need deserialized transaction
					if (!timepointInterval.isInsideInterval(entry->getConfirmedTransaction()->getConfirmedAt().getAsTimepoint())) {
						return FilterResult::DISMISS;
					}
				}
			}
			auto confirmedTransaction = entry->getConfirmedTransaction();
			// auto body = entry->getTransactionBody();
			
			// needing deserialized transaction
			if ((type & FilterCriteria::INVOLVED_PUBLIC_KEY) == FilterCriteria::INVOLVED_PUBLIC_KEY) 
			{
				if (involvedPublicKey && !confirmedTransaction->isInvolved(*involvedPublicKey)) {
					return FilterResult::DISMISS;
				}				
			}	
			if ((type & FilterCriteria::UPDATED_BALANCED_PUBLIC_KEY) == FilterCriteria::UPDATED_BALANCED_PUBLIC_KEY)
			{
				if (updatedBalancePublicKey && !confirmedTransaction->isBalanceUpdated(*updatedBalancePublicKey)) {
					return FilterResult::DISMISS;
				}
			}

			if ((type & FilterCriteria::FILTER_FUNCTION) == FilterCriteria::FILTER_FUNCTION) {
				if (filterFunction) {
					return filterFunction(*entry);
				}
			}
			return FilterResult::USE;
		}

		bool Filter::isTheSame(const Filter& other) const
		{
			if (minTransactionNr != other.minTransactionNr ||
				maxTransactionNr != other.maxTransactionNr ||
				(!involvedPublicKey && other.involvedPublicKey) ||
				(involvedPublicKey && !other.involvedPublicKey) ||
				(involvedPublicKey && !involvedPublicKey->isTheSame(other.involvedPublicKey)) ||
				(!updatedBalancePublicKey && other.updatedBalancePublicKey) ||
				(updatedBalancePublicKey && !other.updatedBalancePublicKey) ||
				(updatedBalancePublicKey && !updatedBalancePublicKey->isTheSame(other.updatedBalancePublicKey)) ||
				searchDirection != other.searchDirection ||
				pagination != other.pagination ||
				coinCommunityIdIndex != other.coinCommunityIdIndex ||
				timepointInterval != other.timepointInterval ||
				transactionType != other.transactionType) {
				return false;
			}
			// cannot really compare filter functions
			if (filterFunction || other.filterFunction) {
				return false;
			}
			return true;
		}
	}
}

