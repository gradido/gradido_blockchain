#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H


#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/ExpireCache.h"
#include "gradido_blockchain/data/Timestamp.h"

#include "TransactionEntry.h"
#include "Filter.h"
#include "SearchDirection.h"
#include "../data/AddressType.h"
#include "../data/TransactionTriggerEvent.h"


#include <list>

namespace gradido {	
	namespace data {
		class LedgerAnchor;
	}
	namespace interaction {
		namespace createConfirmedTransaction {
			class Context;
		}
	}
	namespace blockchain {
		class AbstractProvider;

		class GRADIDOBLOCKCHAIN_EXPORT Abstract
		{
			friend interaction::createConfirmedTransaction::Context;
		public:
			Abstract(uint32_t communityIdIndex);
			virtual ~Abstract() {}

			//! validate and generate confirmed transaction
			//! throw if gradido transaction isn't valid
			//! \return false if transaction already exist
			virtual bool createAndAddConfirmedTransaction(
				data::ConstGradidoTransactionPtr gradidoTransaction, 
				const data::LedgerAnchor& ledgerAnchor,
				data::Timestamp confirmedAt
			) = 0;
			virtual void addTransactionTriggerEvent(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent) = 0;
			virtual void removeTransactionTriggerEvent(const data::TransactionTriggerEvent& transactionTriggerEvent) = 0;

			//! this implementation use findOne so it isn't neccessarly the fastest way of doing this
			virtual bool isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const;

			//! return events in asc order of targetDate
			virtual std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> findTransactionTriggerEventsInRange(TimepointInterval range) = 0;
			virtual std::shared_ptr<const data::TransactionTriggerEvent> findNextTransactionTriggerEventInRange(TimepointInterval range) = 0;

			// main search function, do all the work, reference from other functions
			virtual TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const = 0;
			// find all optimized for counting transaction nrs, better not use the filter.function for that, because this would slow down
			virtual size_t countAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const;
			// only if you expect only one result
			virtual ConstTransactionEntryPtr findOne(const Filter& filter = Filter::LAST_TRANSACTION) const;

			//! analyze only registerAddress Transactions, will use getAddressTypeSlow in basic version
			//! \param use filter to check existing of a address in a subrange of transactions
			//!        check for user and account public keys
			virtual data::AddressType getAddressType(const Filter& filter = Filter::LAST_TRANSACTION) const;
			//! uncached version of getAddressType which will search via findOne in blockchain
			data::AddressType getAddressTypeSlow(const Filter& filter = Filter::LAST_TRANSACTION) const;
			virtual ConstTransactionEntryPtr getTransactionForId(uint64_t transactionId) const = 0;

			//! \param filter use to speed up search if infos exist to narrow down search transactions range
			virtual ConstTransactionEntryPtr findByLedgerAnchor(
				const data::LedgerAnchor& ledgerAnchor,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const;

			inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
			inline data::Timestamp getStartDate() const { return mStartDate; }
			virtual AbstractProvider* getProvider() const = 0;

		protected:
			uint32_t mCommunityIdIndex;
			data::Timestamp mStartDate;			
		};
			
	}
}


#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H