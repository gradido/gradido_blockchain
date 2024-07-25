#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_BUILDER_H

#include "Filter.h"

namespace gradido {
	namespace blockchain {
		class GRADIDOBLOCKCHAIN_EXPORT FilterBuilder
		{
		public:
			inline void reset() { mFilter = Filter(); }
			// default is 0
			inline FilterBuilder& setMinTransactionNr(uint64_t _minTransactionNr) { mFilter.minTransactionNr = _minTransactionNr; return *this;}
			// default is 0, no max use all
			inline FilterBuilder& setMaxTransactionNr(uint64_t _maxTransactionNr) { mFilter.maxTransactionNr = _maxTransactionNr; return *this; }
			// default is nullptr, don't filter for public keys
			inline FilterBuilder& setInvolvedPublicKey(memory::ConstBlockPtr _involvedPublicKey) { mFilter.involvedPublicKey = _involvedPublicKey; return *this; }
			// default is DESC
			inline FilterBuilder& setSearchDirection(SearchDirection _searchDirection) { mFilter.searchDirection = _searchDirection; return *this; }
			// default is no pagination, result all 
			inline FilterBuilder& setPagination(Pagination _pagination) { mFilter.pagination = _pagination; return *this;}
			// default is nullptr, don't filter for coin color
			inline FilterBuilder& setCoinCommunityId(std::string_view _coinCommunityId) { mFilter.coinCommunityId = _coinCommunityId; return *this; }
			// default is zero, don't filter for date
			inline FilterBuilder& setTimepointInterval(TimepointInterval _timepointInterval) { mFilter.timepointInterval = _timepointInterval; return *this; }
			// default is none, don't filter for transaction type
			inline FilterBuilder& setTransactionType(data::TransactionType _transactionType) { mFilter.transactionType = _transactionType; return *this; }

			inline FilterBuilder& setFilterFunction(std::function<FilterResult(const TransactionEntry&)> _filterFunction) {
				mFilter.filterFunction = _filterFunction;
				return *this;
			}

			inline Filter build() { auto f = mFilter; reset(); return f; }
			inline Filter getFilter() const { return mFilter; }

		protected:
			Filter mFilter;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_BUILDER_H