#include "gradido_blockchain/v3_3/blockchain/Filter.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			Filter::Filter()
				: minTransactionNr(0),
				maxTransactionNr(0),
				searchDirection(SearchDirection::DESC),
				filterFunction(nullptr) 
			{
			}

			Filter::Filter(std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction)
				: minTransactionNr(0),
				maxTransactionNr(0),
				searchDirection(SearchDirection::DESC),
				filterFunction(_filterFunction) 
			{
			}

			Filter::Filter(
				uint64_t _minTransactionNr,
				uint64_t _maxTransactionNr,
				memory::ConstBlockPtr _involvedPublicKey = nullptr,
				SearchDirection _searchDirection = SearchDirection::DESC,
				Pagination _pagination = Pagination(0),				
				std::string_view coinCommunityId = nullptr,
				date::month _month = date::month(),
				date::year _year = date::year(),
				std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction = nullptr
			) :
				minTransactionNr(_minTransactionNr),
				maxTransactionNr(_maxTransactionNr),
				involvedPublicKey(_involvedPublicKey),
				searchDirection(_searchDirection),
				pagination(_pagination),				
				coinCommunityId(coinCommunityId),
				month(_month), year(_year), filterFunction(_filterFunction) 
			{
			}

			// constructor for calculate creation sum in validate GradidoCreationRole
			Filter::Filter(
				uint64_t _maxTransactionNr,
				memory::ConstBlockPtr _involvedPublicKey,
				date::month _month,
				date::year _year,
				std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction
			) :
				minTransactionNr(0),
				maxTransactionNr(_maxTransactionNr),
				involvedPublicKey(_involvedPublicKey),
				searchDirection(SearchDirection::DESC),
				month(_month), year(_year),
				filterFunction(_filterFunction)
			{
			}

			// constructor for calculate account balance
			Filter::Filter(
				uint64_t _maxTransactionNr,
				memory::ConstBlockPtr _involvedPublicKey,
				SearchDirection _searchDirection,
				std::string_view coinCommunityId,
				std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction
			) :
				minTransactionNr(0),
				maxTransactionNr(_maxTransactionNr),
				involvedPublicKey(_involvedPublicKey),
				searchDirection(_searchDirection),
				coinCommunityId(coinCommunityId),
				filterFunction(_filterFunction)
			{
			}


			const Filter Filter::LAST_TRANSACTION = Filter(
				0, 0, nullptr, SearchDirection::DESC, Pagination(1, 0)
			);
			const Filter Filter::ALL_TRANSACTIONS = Filter(
				0, 0, nullptr, SearchDirection::DESC, Pagination(0, 0)
			);
		}
	}
}