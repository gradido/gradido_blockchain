#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include "SearchDirection.h"
#include "Pagination.h"
#include "FilterFunctionResult.h"
#include "TransactionEntry.h"

#include "date/date.h"

#include <functional>
#include <cstdint>

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			/*
			* Filter for get specific transactions from blockchain
			* make use of indices to reduce load on gradido node
			*/
			struct GRADIDOBLOCKCHAIN_EXPORT Filter
			{
				Filter();
				Filter(std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction);
				Filter(
					uint64_t _minTransactionNr,
					uint64_t _maxTransactionNr,
					memory::ConstBlockPtr _involvedPublicKey = nullptr,
					SearchDirection _searchDirection = SearchDirection::DESC,
					Pagination _pagination = Pagination(0),					
					std::string_view coinCommunityId = nullptr,
					date::month _month = date::month(),
					date::year _year = date::year(),
					std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction = nullptr
				);
				// constructor for calculate creation sum in validate GradidoCreationRole
				Filter(
					uint64_t _maxTransactionNr,
					memory::ConstBlockPtr _involvedPublicKey,
					date::month _month,
					date::year _year,
					std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction
				);
				// constructor for calculate account balance in interaction::calculateAccountBalance
				Filter(
					uint64_t _maxTransactionNr,
					memory::ConstBlockPtr _involvedPublicKey,
					SearchDirection _searchDirection,
					std::string_view coinCommunityId,
					std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction
				);

				//! transaction number to start from, 0 default
				uint64_t minTransactionNr;
				//! transaction number to stop search, 0 means no stop 
				uint64_t maxTransactionNr;
				//! return only transaction in which the public key is involved, either directly in the transaction or as signer
				memory::ConstBlockPtr involvedPublicKey;
				//! search direction and result order, default: DESC
				SearchDirection searchDirection;
				//! search result scope 
				Pagination pagination;				
				//! for colored coins, default = "" no filtering
				std::string coinCommunityId;
				// TODO: date filter with operator <, >, ==, <=, >=, !=
				//! return only transactions which a confirmed in given month, default = 0 no filtering
				date::month month;
				//! return only transactions which a confirmed in given year, default = 0 no filtering
				date::year year;

				//! for advanced filtering, to prevent unnecessary copy around large amounts of transactions
				//! see FilterFunctionResult for more infos
				std::function<FilterFunctionResult(const TransactionEntry&)> filterFunction;

				static const Filter LAST_TRANSACTION;
				static const Filter ALL_TRANSACTIONS;
			}; 

		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_H