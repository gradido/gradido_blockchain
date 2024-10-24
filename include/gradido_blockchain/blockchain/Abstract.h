#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H

#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/ExpireCache.h"
#include "gradido_blockchain/data/iota/MessageId.h"

#include "TransactionEntry.h"
#include "Filter.h"
#include "SearchDirection.h"
#include "../data/AddressType.h"


#include <list>

namespace gradido {	
	namespace blockchain {
		class AbstractProvider;

		class GRADIDOBLOCKCHAIN_EXPORT Abstract
		{
		public:
			Abstract(std::string_view communityId);
			virtual ~Abstract() {}

			//! validate and generate confirmed transaction
			//! throw if gradido transaction isn't valid
			//! \return false if transaction already exist
			virtual bool addGradidoTransaction(data::ConstGradidoTransactionPtr gradidoTransaction, memory::ConstBlockPtr messageId, Timepoint confirmedAt) = 0;

			// main search function, do all the work, reference from other functions
			virtual TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const = 0;
			// only if you expect only one result
			std::shared_ptr<const TransactionEntry> findOne(const Filter& filter = Filter::LAST_TRANSACTION) const;

			//! find all deferred transfers which have the timeout in date range between start and end, have senderPublicKey and are not redeemed,
			//! therefore boocked back to sender
			//! find all deferred transfers which have the timeout in date range between start and end, have senderPublicKey and are not redeemed,
			//! therefore boocked back to sender
			virtual TransactionEntries findTimeoutedDeferredTransfersInRange(
				memory::ConstBlockPtr senderPublicKey,
				TimepointInterval timepointInterval,
				uint64_t maxTransactionNr
			) const =  0;

			//! find all transfers which redeem a deferred transfer in date range
			//! \param senderPublicKey sender public key of sending account of deferred transaction
			//! \return list with transaction pairs, first is deferred transfer, second is redeeming transfer
			virtual std::list<DeferredRedeemedTransferPair> findRedeemedDeferredTransfersInRange(
				memory::ConstBlockPtr senderPublicKey,
				TimepointInterval timepointInterval,
				uint64_t maxTransactionNr
			) const = 0;

			//! analyze only registerAddress Transactions
			//! \param use filter to check existing of a address in a subrange of transactions
			//!        check for user and account public keys
			data::AddressType getAddressType(const Filter& filter = Filter::ALL_TRANSACTIONS) const;
			virtual std::shared_ptr<const TransactionEntry> getTransactionForId(uint64_t transactionId) const = 0;

			//! \param filter use to speed up search if infos exist to narrow down search transactions range
			virtual std::shared_ptr<const TransactionEntry> findByMessageId(
				memory::ConstBlockPtr messageId,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const;

			inline std::string_view getCommunityId() const { return mCommunityId; }
			inline Timepoint getStartDate() const { return mStartDate; }
			virtual AbstractProvider* getProvider() const = 0;

		protected:
			std::string mCommunityId;
			Timepoint mStartDate;
			
		};
			
	}
}


#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H