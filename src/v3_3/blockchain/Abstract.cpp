#include "gradido_blockchain/v3_3/blockchain/Abstract.h"
#include "gradido_blockchain/v3_3/blockchain/Exceptions.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			std::shared_ptr<TransactionEntry> Abstract::findOne(const Filter& filter = Filter::LAST_TRANSACTION) 
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

			DecayDecimal Abstract::calculateAddressBalance(
				memory::ConstBlockPtr accountPublicKey,
				Timepoint date,
				const Filter& filter = Filter::ALL_TRANSACTIONS
			)
			{

			}

			std::shared_ptr<TransactionEntry> Abstract::findByMessageId(
				memory::ConstBlockPtr messageId,
				bool cachedOnly,/* = true */
				const Filter& filter /*= Filter::ALL_TRANSACTIONS*/
			)
			{
				auto result = mMessageIdTransactionNrCache.get(iota::MessageId::fromMemoryBlock(*messageId));
				if (result.has_value()) {
					return getTransactionForId(result.value());
				}
				if (cachedOnly) return nullptr;
				// copy filter
				Filter f(filter);
				f.filterFunction = [&messageId, filter](const TransactionEntry& entry) -> FilterFunctionResult {
					if (filter.filterFunction) {
						// evaluate filter from caller
						auto result = filter.filterFunction(entry);
						if ((result & FilterFunctionResult::USE) != FilterFunctionResult::USE) {
							return result;
						}
					}
					if (messageId->isTheSame(entry.getConfirmedTransaction()->messageId)) {
						return FilterFunctionResult::USE | FilterFunctionResult::STOP;
					}
				};
				return findOne(f);
			}
			data::AddressType Abstract::getAddressType(memory::ConstBlockPtr publicKey, const Filter& filter/* = Filter::ALL_TRANSACTIONS */)
			{
				// copy filter
				Filter f(filter);
				f.filterFunction = [&publicKey, filter](const TransactionEntry& entry) -> FilterFunctionResult {
					if (filter.filterFunction) {
						// evaluate filter from caller
						auto result = filter.filterFunction(entry);
						if ((result & FilterFunctionResult::USE) != FilterFunctionResult::USE) {
							return result;
						}
					}
					auto body = entry.getConfirmedTransaction()->gradidoTransaction->getTransactionBody();
					if (body->isRegisterAddress() && body->isInvolved(publicKey)) {
						return FilterFunctionResult::USE | FilterFunctionResult::STOP;
					}
				};
				auto transactionEntry = findOne(f);
				if (transactionEntry) {
					return transactionEntry->getConfirmedTransaction()->gradidoTransaction->getTransactionBody()->registerAddress->addressType;
				}
				return data::AddressType::NONE;
			}
		}

	}
}