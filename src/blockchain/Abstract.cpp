#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"

namespace gradido {
	namespace blockchain {

		Abstract::Abstract(uint32_t communityIdIndex)
			: mCommunityIdIndex(communityIdIndex)
		{

		}

		bool Abstract::isTransactionExist(data::ConstGradidoTransactionPtr gradidoTransaction) const
		{
			const auto& body = gradidoTransaction->getTransactionBody();
			FilterBuilder builder;
			return findOne(builder
				.setTransactionType(body->getTransactionType())
				.setTimepointInterval(TimepointInterval(body->getCreatedAt()))
				.setFilterFunction([gradidoTransaction](const TransactionEntry& entry) -> FilterResult {
					const auto& otherGradidoTransaction = entry.getConfirmedTransaction()->getGradidoTransaction();
					if (gradidoTransaction->isTheSame(*otherGradidoTransaction)) {
						return FilterResult::USE | FilterResult::STOP;
					}
					return FilterResult::DISMISS;
				}).build()
			) != nullptr;
		}

		size_t Abstract::countAll(const Filter& filter/* = Filter::ALL_TRANSACTIONS*/) const
		{
			return findAll(filter).size();
		}

		std::shared_ptr<const TransactionEntry> Abstract::findOne(const Filter& filter/* = Filter::LAST_TRANSACTION*/) const
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

			
		data::AddressType Abstract::getAddressType(const Filter& filter/* = Filter::LAST_TRANSACTION */) const
		{
			return getAddressTypeSlow(filter);
		}

		data::AddressType Abstract::getAddressTypeSlow(const Filter& filter/* = Filter::LAST_TRANSACTION */) const
		{
			if (!filter.involvedPublicKey) {
				throw GradidoNodeInvalidDataException("involvedPublicKey must be set in filter for searching for address type");
			}
			auto firstTransaction = findOne(Filter::FIRST_TRANSACTION);
			if (!firstTransaction) return data::AddressType::NONE;
			assert(firstTransaction->getTransactionBody()->isCommunityRoot());
			auto communityRoot = firstTransaction->getTransactionBody()->getCommunityRoot().value();
			if (filter.involvedPublicKey) {
				assert(filter.involvedPublicKey->size() == 32);
				auto involvedPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(mCommunityIdIndex, { filter.involvedPublicKey->data() });
				if (communityRoot.aufPublicKeyIndex.publicKeyIndex == involvedPublicKeyIndex) {
					return data::AddressType::COMMUNITY_AUF;
				}
				else if (communityRoot.gmwPublicKeyIndex.publicKeyIndex == involvedPublicKeyIndex) {
					return data::AddressType::COMMUNITY_GMW;
				}
			}

			// copy filter
			Filter f(filter);
			f.transactionType = data::TransactionType::REGISTER_ADDRESS;
			f.pagination.size = 1;
			// need be started from back because of moving
			f.searchDirection = SearchDirection::DESC;
			auto transactionEntry = findOne(f);
			if (transactionEntry) {
				return transactionEntry->getTransactionBody()->getRegisterAddress()->getAddressType();
			}
			// check for deferred transfer transaction
			f.transactionType = data::TransactionType::DEFERRED_TRANSFER;
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

		std::shared_ptr<const TransactionEntry> Abstract::findByLedgerAnchor(
			const data::LedgerAnchor& ledgerAnchor,
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