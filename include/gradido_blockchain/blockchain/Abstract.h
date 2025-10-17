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
			Abstract(std::string_view communityId);
			virtual ~Abstract() {}

			//! validate and generate confirmed transaction
			//! throw if gradido transaction isn't valid
			//! \return false if transaction already exist
			virtual bool createAndAddConfirmedTransaction(data::ConstGradidoTransactionPtr gradidoTransaction, memory::ConstBlockPtr messageId, data::Timestamp confirmedAt) = 0;
			virtual void addTransactionTriggerEvent(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent) = 0;
			virtual void removeTransactionTriggerEvent(const data::TransactionTriggerEvent& transactionTriggerEvent) = 0;

			//! this implementation use findOne so it isn't neccessarly the fastest way of doing this
			virtual bool isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const;

			//! return events in asc order of targetDate
			virtual std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> findTransactionTriggerEventsInRange(TimepointInterval range) = 0;

			// main search function, do all the work, reference from other functions
			virtual TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const = 0;
			// only if you expect only one result
			std::shared_ptr<const TransactionEntry> findOne(const Filter& filter = Filter::LAST_TRANSACTION) const;

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
			inline data::Timestamp getStartDate() const { return mStartDate; }
			virtual AbstractProvider* getProvider() const = 0;

		protected:
			std::string mCommunityId;
			data::Timestamp mStartDate;			
		};
			
	}
}


#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H