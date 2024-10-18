#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include "SearchDirection.h"
#include "Pagination.h"
#include "FilterResult.h"
#include "FilterCriteria.h"
#include "TransactionEntry.h"
#include "gradido_blockchain/lib/TimepointInterval.h"

#include <functional>
#include <cstdint>

namespace gradido {
	namespace blockchain {
		/*
		* Filter for get specific transactions from blockchain
		* make use of indices to reduce load on gradido node
		*/
		struct GRADIDOBLOCKCHAIN_EXPORT Filter
		{
			Filter();
			Filter(std::function<FilterResult(const TransactionEntry&)> _filterFunction);
			Filter(
				uint64_t _minTransactionNr,
				uint64_t _maxTransactionNr,
				memory::ConstBlockPtr _involvedPublicKey = nullptr,
				SearchDirection _searchDirection = SearchDirection::DESC,
				Pagination _pagination = Pagination(0),					
				std::string_view coinCommunityId = std::string_view(),
				TimepointInterval _timepointInterval = TimepointInterval(),
				data::TransactionType _transactionType = data::TransactionType::NONE,
				std::function<FilterResult(const TransactionEntry&)> _filterFunction = nullptr
			);
			// constructor for calculate creation sum in validate GradidoCreationRole
			Filter(
				uint64_t _maxTransactionNr,
				memory::ConstBlockPtr _involvedPublicKey,
				TimepointInterval _timepointInterval,
				std::function<FilterResult(const TransactionEntry&)> _filterFunction
			);
			// constructor for calculate account balance in interaction::calculateAccountBalance
			Filter(
				uint64_t _maxTransactionNr,
				memory::ConstBlockPtr _involvedPublicKey,
				SearchDirection _searchDirection,
				std::string_view coinCommunityId,
				std::function<FilterResult(const TransactionEntry&)> _filterFunction
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
			//! interval between two dates with 1 month resolution
			TimepointInterval timepointInterval;
			//! transaction type
			data::TransactionType transactionType;

			//! for advanced filtering, to prevent unnecessary copy around large amounts of transactions
			//! see FilterFunctionResult for more infos
			std::function<FilterResult(const TransactionEntry&)> filterFunction;

			//! check if transaction fulfill filter type
			//! \param entry transaction to match
			//! \param type filter criteria to check			
			FilterResult matches(std::shared_ptr<const TransactionEntry> entry, FilterCriteria type, std::string_view communityId) const;

			static const Filter LAST_TRANSACTION;
			static const Filter ALL_TRANSACTIONS;
		}; 
	}
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_H