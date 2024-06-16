#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_BY_INDICES_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_BY_INDICES_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include <cstdint>

#include "SearchDirection.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			/*
			* Filter for get specific transactions from blockchain
			* make use of indices to reduce load on gradido node
			*/
			struct GRADIDOBLOCKCHAIN_EXPORT FilterByIndices
			{
				FilterByIndices()
					: minTransactionNr(0),
					maxTransactionNr(0),
					searchDirection(SearchDirection::ASC),
					coinCommunityId(""),
					month(0), year(0) {}

				//! transaction nr to start from, 0 default
				uint64_t minTransactionNr;
				//! transaction nr to stop search, 0 means no stop 
				uint64_t maxTransactionNr;
				//! search direction and result order, default: ASC
				SearchDirection searchDirection;
				//! return only transaction in which the public key is involved, either directly in the transaction or as signer
				memory::ConstBlockPtr involvedPublicKey;
				//! for colored coins, default = "" no filtering
				std::string coinCommunityId;
				//! return only transactions which a confirmed in given month, default = 0 no filtering
				int month;
				//! return only transactions which a confirmed in given year, default = 0 no filtering
				int year;
			};

		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_BY_INDICES_H