#ifndef GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H
#define GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H

#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "gradido_blockchain/types.h"
#include "CompactPagination.h"
#include "CommunityIdType.h"
#include "Pagination.h"
#include "SearchDirection.h"
#include "Filter.h"
#include "PublicKeySearchType.h"
#include "FilterResult.h"

namespace gradido {
	namespace data::compact {
		struct ConfirmedGradidoTx;
	}
	namespace blockchain {
		
		struct GRADIDOBLOCKCHAIN_EXPORT CompactFilter
		{
			CompactFilter();
			CompactFilter(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary);
			//! transaction number to start from, 0 default
			uint64_t minTransactionNr;
			//! transaction number to stop search, 0 means no stop 
			uint64_t maxTransactionNr;
			//! return only transaction in which the public key is involved, either directly in the transaction or as signer
			uint32_t publicKeyIndex;
			
			//! for colored coins, index starts with 0 so to check if it is actuall set, check hasCoinCommunityIndex
			uint32_t coinCommunityIdIndex;

			//! transaction type
			data::TransactionType transactionType;
			//! search direction and result order, default: DESC
			SearchDirection searchDirection;
			//! type of data publicKeyIndex contains
			PublicKeySearchType publicKeySearchType;			
			//! if true, coinCommunityIdIndex has a value
			CommunityIdType communityIdType;
			//! search result scope 
			CompactPagination pagination; 
			//! interval between two dates with 1 month resolution
			TimepointInterval timepointInterval;

			FilterResult matches(const data::compact::ConfirmedGradidoTx& confirmedTx, FilterCriteria type) const;
		};
	}
}

#endif // GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H