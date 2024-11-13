#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Exceptions.h"

namespace gradido {
	namespace blockchain {

		Abstract::Abstract(std::string_view communityId)
			: mCommunityId(communityId)
		{

		}

		std::shared_ptr<const TransactionEntry> Abstract::findOne(const Filter& filter/* = Filter::LAST_TRANSACTION*/) const
		{
			auto results = findAll(filter);
			if (!results.size()) { 
				return nullptr; 
			}
			if (results.size() > 1) {
				throw TransactionResultCountException("to many transactions found with blockchain::Abstract::findOne", 1, results.size());
			}
			return results.front();
		}

			
		data::AddressType Abstract::getAddressType(const Filter& filter/* = Filter::ALL_TRANSACTIONS */) const
		{
			if (!filter.involvedPublicKey) {
				throw GradidoNodeInvalidDataException("involvedPublicKey must be set in filter for searching for address type");
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

		std::shared_ptr<const TransactionEntry> Abstract::findByMessageId(
			memory::ConstBlockPtr messageId,
			const Filter& filter /*= Filter::ALL_TRANSACTIONS*/
		) const
		{
			// copy filter
			Filter f(filter);
			f.filterFunction = [&messageId, filter](const TransactionEntry& entry) -> FilterResult {
				if (filter.filterFunction) {
					// evaluate filter from caller
					auto result = filter.filterFunction(entry);
					if ((result & FilterResult::USE) != FilterResult::USE) {
						return result;
					}
				}
				if (messageId->isTheSame(entry.getConfirmedTransaction()->getMessageId())) {
					return FilterResult::USE | FilterResult::STOP;
				}
				return FilterResult::DISMISS;
				};
			return findOne(f);
		}
	}
}