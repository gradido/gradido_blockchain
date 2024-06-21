#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_H

#include "gradido_blockchain/types.h"
#include "gradido_blockchain/lib/DecayDecimal.h"
#include "gradido_blockchain/lib/ExpireCache.h"
#include "gradido_blockchain/data/iota/MessageId.h"

#include "TransactionEntry.h"
#include "Filter.h"
#include "SearchDirection.h"
#include "../data/AddressType.h"


#include <list>

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			typedef std::list<std::shared_ptr<TransactionEntry>> TransactionEntries;
			class Abstract
			{
			public:
				// main search function, do all the work, reference from all other
				virtual TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) = 0;
				// only if you expect only one result
				std::shared_ptr<TransactionEntry> findOne(const Filter& filter = Filter::LAST_TRANSACTION);

				//! calculate account address balance at date with decay
				//! \param filter for exclude transactions, search for last transaction with final balance and calculate from there + decay
				//! make use of final_balance so balance always start by first transaction of user
				DecayDecimal calculateAddressBalance(
					memory::ConstBlockPtr accountPublicKey,
					Timepoint date,
					const Filter& filter = Filter::ALL_TRANSACTIONS
				);
				//! analyze only registerAddress Transactions
				//! \param use filter to check existing of a address in a subrange of transactions
				//!        check for user and account public keys
				data::AddressType getAddressType(memory::ConstBlockPtr publicKey, const Filter& filter = Filter::ALL_TRANSACTIONS);
				virtual std::shared_ptr<TransactionEntry> getTransactionForId(uint64_t transactionId) = 0;

				//! use a cache for message ids for lastly added message ids
				//! \param cachedOnly if true check only the expire cache
				//! \param filter use to speed up search if infos exist to narrow down search transactions range
				std::shared_ptr<TransactionEntry> findByMessageId(
					memory::ConstBlockPtr messageId,
					bool cachedOnly = true,
					const Filter& filter = Filter::ALL_TRANSACTIONS
				);

				inline std::string_view getCommunityId() const { mCommunityId; }

			protected:
				ExpireCache<iota::MessageId, uint64_t> mMessageIdTransactionNrCache;
				std::string mCommunityId;
			};
			
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_H