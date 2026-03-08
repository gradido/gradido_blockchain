#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/TransactionType.h"

#include <memory>

using std::shared_ptr;

namespace gradido {
	using data::AddressType, data::LedgerAnchor, data::TransactionType;
	using data::compact::ConfirmedGradidoTx, data::compact::ConstConfirmedTxPtr;
	using data::ConstGradidoTransactionPtr;

	namespace blockchain {

		Abstract::Abstract(uint32_t communityIdIndex)
			: mCommunityIdIndex(communityIdIndex)
		{

		}

		bool Abstract::isTransactionExist(ConstGradidoTransactionPtr gradidoTransaction, data::Timestamp confirmedAt) const
		{
			const auto& body = gradidoTransaction->getTransactionBody();
			Filter f;
			f.searchDirection = SearchDirection::DESC;
			f.timepointInterval = TimepointInterval(confirmedAt);
			f.transactionType = body->getTransactionType();
			f.filterFunction = 
				[&gradidoTransaction](const TransactionEntry& entry) -> FilterResult
				{
					const auto& otherGradidoTransaction = entry.getConfirmedTransaction()->getGradidoTransaction();
					if (gradidoTransaction->isTheSame(*otherGradidoTransaction)) {
						return FilterResult::USE | FilterResult::STOP;
					}
					return FilterResult::DISMISS;
				}
			;
			
			return findOne(f) != nullptr;
		}

		size_t Abstract::countAll(const Filter& filter/* = Filter::ALL_TRANSACTIONS*/) const
		{
			return findAll(filter).size();
		}

		size_t Abstract::countAll(const CompactFilter& filter) const
		{
			return findAll(filter).size();
		}

		shared_ptr<const TransactionEntry> Abstract::findOne(const Filter& filter/* = Filter::LAST_TRANSACTION*/) const
		{
			auto results = findAll(filter);
			if (!results.size()) { 
				return nullptr; 
			}
			if (results.size() > 1) {
				throw TransactionResultCountException(
					"to many transactions found with blockchain::Abstract::findOne", 
					1, 
					results.size(),
					filter
				);
			}
			return results.front();
		}

		ConstConfirmedTxPtr Abstract::findOne(const CompactFilter& filter) const
		{
			auto results = findAll(filter);
			if (!results.size()) {
				 return nullptr;
				// return nullptr;
			}
			if (results.size() > 1) {
				throw TransactionResultCountException(
					"to many transactions found with blockchain::Abstract::findOne",
					1,
					results.size(),
					filter
				);
			}
			return results.front();
		}
			
		AddressType Abstract::getAddressType(const Filter& filter/* = Filter::LAST_TRANSACTION */) const
		{
			return getAddressTypeSlow(filter);
		}
		data::AddressType Abstract::getAddressType(const CompactFilter& filter) const
		{
			return getAddressTypeSlow(filter);
		}

		AddressType Abstract::getAddressTypeSlow(const Filter& filter/* = Filter::LAST_TRANSACTION */) const
		{
			if (!filter.involvedPublicKey) {
				throw GradidoNodeInvalidDataException("involvedPublicKey must be set in filter for searching for address type");
			}
			auto firstTransaction = findOne(Filter::FIRST_TRANSACTION);
			if (!firstTransaction) return AddressType::NONE;
			assert(firstTransaction->getTransactionBody()->isCommunityRoot());
			auto communityRoot = firstTransaction->getTransactionBody()->getCommunityRoot().value();
			if (filter.involvedPublicKey) {
				assert(filter.involvedPublicKey->size() == 32);
				auto involvedPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(mCommunityIdIndex, { filter.involvedPublicKey->data() });
				if (communityRoot.aufPublicKeyIndex == involvedPublicKeyIndex) {
					return AddressType::COMMUNITY_AUF;
				}
				else if (communityRoot.gmwPublicKeyIndex == involvedPublicKeyIndex) {
					return AddressType::COMMUNITY_GMW;
				}
			}

			// copy filter
			Filter f(filter);
			f.transactionType = TransactionType::REGISTER_ADDRESS;
			f.pagination.size = 1;
			// need be started from back because of moving
			f.searchDirection = SearchDirection::DESC;
			auto transactionEntry = findOne(f);
			if (transactionEntry) {
				return transactionEntry->getTransactionBody()->getRegisterAddress()->addressType;
			}
			// check for deferred transfer transaction
			f.transactionType = TransactionType::DEFERRED_TRANSFER;
			f.filterFunction = [f](const TransactionEntry& entry) -> FilterResult {
				if (!f.involvedPublicKey->isTheSame(entry.getTransactionBody()->getDeferredTransfer()->getRecipientPublicKey())) {
					return FilterResult::DISMISS;
				}
				else {
					return FilterResult::USE;
				}
			};
			transactionEntry = findOne(f);
			if (transactionEntry) {
				return data::AddressType::DEFERRED_TRANSFER;
			}
			return data::AddressType::NONE;
		}

		data::AddressType Abstract::getAddressTypeSlow(const CompactFilter& filter) const
		{
			if (PublicKeySearchType::InvolvedPublicKey != filter.publicKeySearchType || filter.publicKeyIndex.empty()) {
				throw GradidoNodeInvalidDataException("involvedPublicKey must be set in filter for searching for address type");
			}
			auto firstTransaction = findOne(CompactFilter::firstTransaction());
			if (!firstTransaction) return AddressType::NONE;
			assert(firstTransaction->isCommunityRoot());
			
			if (firstTransaction->getAuf() == filter.publicKeyIndex) {
				return AddressType::COMMUNITY_AUF;
			}
			else if (firstTransaction->getGmw() == filter.publicKeyIndex) {
				return AddressType::COMMUNITY_GMW;
			}

			// copy filter
			CompactFilter f(filter);
			f.transactionType = TransactionType::REGISTER_ADDRESS;
			f.pagination.size = 1;
			// need be started from back because of moving
			f.searchDirection = SearchDirection::DESC;
			auto transactionEntry = findOne(f);
			if (transactionEntry) {
				return transactionEntry->specific.registerAddress.addressType;
			}
			// check for deferred transfer transaction
			f.transactionType = TransactionType::DEFERRED_TRANSFER;
			auto transactionEntries = findAll(f, 
				[f](const ConfirmedGradidoTx& entry) -> FilterResult 
				{
					if (f.publicKeyIndex != entry.getRecipient()) {
						return FilterResult::DISMISS;
					}
					else {
						return FilterResult::USE | FilterResult::STOP;
					}
				}
			);
			if (transactionEntries.size()) {
				return data::AddressType::DEFERRED_TRANSFER;
			}
			return data::AddressType::NONE;
		}

		shared_ptr<const TransactionEntry> Abstract::findByLedgerAnchor(
			const LedgerAnchor& ledgerAnchor,
			const Filter& filter /*= Filter::ALL_TRANSACTIONS*/
		) const
		{
			// copy filter
			Filter f(filter);
			f.filterFunction = [&ledgerAnchor, filter](const TransactionEntry& entry) -> FilterResult {
				if (filter.filterFunction) {
					// evaluate filter from caller
					auto result = filter.filterFunction(entry);
					if ((result & FilterResult::USE) != FilterResult::USE) {
						return result;
					}
				}
				if (ledgerAnchor.isTheSame(entry.getConfirmedTransaction()->getLedgerAnchor())) {
					return FilterResult::USE | FilterResult::STOP;
				}
				return FilterResult::DISMISS;
				};
			return findOne(f);
		}
	}
}