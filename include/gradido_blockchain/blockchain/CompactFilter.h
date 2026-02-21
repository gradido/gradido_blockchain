#ifndef GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H
#define GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H

#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "gradido_blockchain/types.h"
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
			CompactFilter(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary, uint32_t communityIdIndex = 0);

			//! search direction and result order, default: DESC
			SearchDirection searchDirection;

			//! transaction type
			data::TransactionType transactionType;

			//! type of data publicKeyIndex contains
			PublicKeySearchType publicKeySearchType;

			//! for colored coins, index starts with 1
			uint32_t coinCommunityIdIndex;

			//! transaction number to stop search, 0 means no stop 
			uint64_t maxTransactionNr;
			//! transaction number to start from, 0 default
			uint64_t minTransactionNr;

			//! return only transaction in which the public key is involved, either directly in the transaction or as signer
			data::compact::PublicKeyIndex publicKeyIndex;

			//! search result scope 
			Pagination pagination; 
			//! interval between two dates with 1 month resolution
			TimepointInterval timepointInterval;

			static CompactFilter lastBalanceFor(data::compact::PublicKeyIndex publicKeyIndex);

			FilterResult matches(const data::compact::ConfirmedGradidoTx& confirmedTx, FilterCriteria type) const;
		};
	}
}

#endif // GRADIDO_BLOCKCHAIN_COMPACT_FILTER_H