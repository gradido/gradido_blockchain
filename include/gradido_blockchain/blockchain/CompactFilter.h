#ifndef GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H
#define GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H

#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "gradido_blockchain/types.h"
#include "CompactPagination.h"
#include "Pagination.h"
#include "SearchDirection.h"
#include "Filter.h"
#include "FilterResult.h"

namespace gradido {
	namespace data::compact {
		// struct 
	}
	namespace blockchain {
		enum PublicKeySearchType : uint8_t {
			None,
			InvolvedPublicKey,
			BalanceChangingPublicKey,
			MissingIndex
		};
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
			
			//! for colored coins, default = "" no filtering
			uint32_t coinCommunityIdIndex;

			//! transaction type
			data::TransactionType transactionType;
			SearchDirection searchDirection;
			PublicKeySearchType publicKeySearchType;			
			bool hasCoinCommunityIndex;
			CompactPagination pagination;
		};
	}
}

#endif // GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H