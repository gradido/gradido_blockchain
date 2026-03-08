#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "gradido_blockchain/data/Timestamp.h"

#include "Filter.h"
#include "../data/AddressType.h"
#include "../data/TransactionTriggerEvent.h"
// #include "../data/compact/ConfirmedGradidoTx.h"

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <vector>

namespace gradido {	
	namespace data {
		class AccountBalance;
		class GradidoTransaction;
		class LedgerAnchor;

		using ConstGradidoTransactionPtr = std::shared_ptr<const GradidoTransaction>;
		namespace compact {
			struct ConfirmedGradidoTx;
			using ConstConfirmedTxPtr = std::shared_ptr<const ConfirmedGradidoTx>;
			using ConfirmedTxs = std::vector<ConstConfirmedTxPtr>;
		}
	}
	namespace interaction {
		namespace createConfirmedTransaction {
			class Context;
		}
	}
	namespace blockchain {
		class AbstractProvider;
		// class CompactFilter;
		class TransactionEntry;
		using ConstTransactionEntryPtr = std::shared_ptr<const TransactionEntry>;
		using TransactionEntries = std::vector<ConstTransactionEntryPtr>;

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

			virtual bool createAndAddConfirmedTransactionExtern(
				data::ConstGradidoTransactionPtr gradidoTransaction,
				const data::LedgerAnchor& ledgerAnchor,
				std::vector<data::AccountBalance> accountBalances
			) = 0;
			virtual void addTransactionTriggerEvent(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent) = 0;
			virtual void removeTransactionTriggerEvent(const data::TransactionTriggerEvent& transactionTriggerEvent) = 0;

			//! this implementation use findOne so it isn't neccessarly the fastest way of doing this
			virtual bool isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction, data::Timestamp confirmedAt) const;

			//! return events in asc order of targetDate
			virtual std::vector<std::shared_ptr<const data::TransactionTriggerEvent>> findTransactionTriggerEventsInRange(data::Timestamp startDate, data::Timestamp endDate) = 0;
			virtual std::shared_ptr<const data::TransactionTriggerEvent> findNextTransactionTriggerEventInRange(data::Timestamp startDate, data::Timestamp endDate) = 0;

			// main search function, do all the work, reference from other functions
			virtual TransactionEntries findAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const = 0;
			virtual data::compact::ConfirmedTxs findAll(
				const CompactFilter& filter, 
				std::function<FilterResult(const data::compact::ConfirmedGradidoTx&)> filterFunction = nullptr
			) const = 0;
			// find all optimized for counting transaction nrs, better not use the filter.function for that, because this would slow down
			virtual size_t countAll(const Filter& filter = Filter::ALL_TRANSACTIONS) const;
			virtual size_t countAll(const CompactFilter& filter) const;
			// only if you expect only one result
			virtual ConstTransactionEntryPtr findOne(const Filter& filter = Filter::LAST_TRANSACTION) const;
			virtual data::compact::ConstConfirmedTxPtr findOne(const CompactFilter& filter) const;

			//! analyze only registerAddress Transactions, will use getAddressTypeSlow in basic version
			//! \param use filter to check existing of a address in a subrange of transactions
			//!        check for user and account public keys
			virtual data::AddressType getAddressType(const Filter& filter = Filter::LAST_TRANSACTION) const;
			virtual data::AddressType getAddressType(const CompactFilter& filter) const;
			//! uncached version of getAddressType which will search via findOne in blockchain
			//! TODO: better name
			data::AddressType getAddressTypeSlow(const Filter& filter = Filter::LAST_TRANSACTION) const;
			data::AddressType getAddressTypeSlow(const CompactFilter& filter) const;
			virtual ConstTransactionEntryPtr getTransactionForId(uint64_t transactionId) const = 0;
			virtual data::compact::ConstConfirmedTxPtr getConfirmedTxForId(uint64_t transactionId) const = 0;

			//! \param filter use to speed up search if infos exist to narrow down search transactions range
			virtual ConstTransactionEntryPtr findByLedgerAnchor(
				const data::LedgerAnchor& ledgerAnchor,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			) const;

			inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
			inline data::Timestamp getStartDate() const { return mStartDate; }
			virtual AbstractProvider* getProvider() const = 0;
			virtual const IDictionary<PublicKey>& getPublicKeyDictionary() const = 0;
			virtual uint32_t getOrAddPublicKey(const PublicKey& publicKey) = 0;

		protected:
			uint32_t mCommunityIdIndex;
			data::Timestamp mStartDate;
		};

	}
}


#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_H