#include "gradido_blockchain/blockchain/Filter.h"

namespace gradido {
	namespace blockchain {

		Filter::Filter()
			: minTransactionNr(0),
			maxTransactionNr(0),
			searchDirection(SearchDirection::DESC),
			transactionType(data::TransactionType::NONE),
			filterFunction(nullptr)			
		{
		}

		Filter::Filter(std::function<FilterResult(const TransactionEntry&)> _filterFunction)
			: minTransactionNr(0),
			maxTransactionNr(0),
			searchDirection(SearchDirection::DESC),
			transactionType(data::TransactionType::NONE),
			filterFunction(_filterFunction)			
		{
		}

		Filter::Filter(
			uint64_t _minTransactionNr,
			uint64_t _maxTransactionNr,
			memory::ConstBlockPtr _involvedPublicKey /*= nullptr*/,
			SearchDirection _searchDirection /*= SearchDirection::DESC*/,
			Pagination _pagination /*= Pagination(0)*/,
			std::optional<uint32_t> _coinCommunityIdIndex /*= std::nullopt() */,
			TimepointInterval _timepointInterval/* = MonthYearInterval()*/,
			data::TransactionType _transactionType /* = data::TransactionType::NONE*/,
			std::function<FilterResult(const TransactionEntry&)> _filterFunction/* = nullptr*/
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
			memory::ConstBlockPtr _involvedPublicKey,
			TimepointInterval _timepointInterval,
			std::function<FilterResult(const TransactionEntry&)> _filterFunction
		) :
			minTransactionNr(0),
			maxTransactionNr(_maxTransactionNr),
			involvedPublicKey(_involvedPublicKey),
			searchDirection(SearchDirection::DESC),
			timepointInterval(_timepointInterval),
			transactionType(data::TransactionType::NONE),
			filterFunction(_filterFunction)
		{
		}

		// constructor for calculate account balance
		Filter::Filter(
			uint64_t _maxTransactionNr,
			memory::ConstBlockPtr _involvedPublicKey,
			SearchDirection _searchDirection,
			std::optional<uint32_t> _coinCommunityIdIndex,
			std::function<FilterResult(const TransactionEntry&)> _filterFunction
		) :
			minTransactionNr(0),
			maxTransactionNr(_maxTransactionNr),
			involvedPublicKey(_involvedPublicKey),
			searchDirection(_searchDirection),
			coinCommunityIdIndex(_coinCommunityIdIndex),
			transactionType(data::TransactionType::NONE),
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

		FilterResult Filter::matches(std::shared_ptr<const TransactionEntry> entry, FilterCriteria type) const
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
				if (transactionType != data::TransactionType::NONE) {
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
				!involvedPublicKey->isTheSame(other.involvedPublicKey) ||
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

