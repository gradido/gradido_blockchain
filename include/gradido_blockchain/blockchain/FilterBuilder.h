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
			inline FilterBuilder& setMinTransactionNr(uint64_t minTransactionNr) { mFilter.minTransactionNr = minTransactionNr; return *this;}
			// default is 0, no max use all
			inline FilterBuilder& setMaxTransactionNr(uint64_t maxTransactionNr) { mFilter.maxTransactionNr = maxTransactionNr; return *this; }
			// default is nullptr, don't filter for public keys
			inline FilterBuilder& setInvolvedPublicKey(memory::ConstBlockPtr involvedPublicKey) { mFilter.involvedPublicKey = involvedPublicKey; return *this; }
			// default is nullptr
			inline FilterBuilder& setUpdatedBalancePublicKey(memory::ConstBlockPtr updatedBalancePublicKey) { mFilter.updatedBalancePublicKey = updatedBalancePublicKey; return *this; }
			// default is DESC
			inline FilterBuilder& setSearchDirection(SearchDirection searchDirection) { mFilter.searchDirection = searchDirection; return *this; }
			// default is no pagination, result all 
			inline FilterBuilder& setPagination(Pagination pagination) { mFilter.pagination = pagination; return *this;}
			// default is nullptr, don't filter for coin color
			inline FilterBuilder& setCoinCommunityId(std::string_view coinCommunityId) { mFilter.coinCommunityId = coinCommunityId; return *this; }
			// default is zero, don't filter for date
			inline FilterBuilder& setTimepointInterval(TimepointInterval timepointInterval) { mFilter.timepointInterval = timepointInterval; return *this; }
			// default is none, don't filter for transaction type
			inline FilterBuilder& setTransactionType(data::TransactionType transactionType) { mFilter.transactionType = transactionType; return *this; }

			inline FilterBuilder& setFilterFunction(std::function<FilterResult(const TransactionEntry&)> filterFunction) {
				mFilter.filterFunction = filterFunction;
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