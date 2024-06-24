#include "gradido_blockchain/v3_3/blockchain/Abstract.h"
#include "gradido_blockchain/v3_3/blockchain/Exceptions.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			Abstract::Abstract(std::string_view communityId)
				: mCommunityId(communityId)
			{

			}

			std::shared_ptr<TransactionEntry> Abstract::findOne(const Filter& filter/* = Filter::LAST_TRANSACTION*/) const
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

			
			data::AddressType Abstract::getAddressType(memory::ConstBlockPtr publicKey, const Filter& filter/* = Filter::ALL_TRANSACTIONS */) const
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
					return FilterFunctionResult::DISMISS;
				};
				auto transactionEntry = findOne(f);
				if (transactionEntry) {
					return transactionEntry->getConfirmedTransaction()->gradidoTransaction->getTransactionBody()->registerAddress->addressType;
				}
				return data::AddressType::NONE;
			}

			std::shared_ptr<TransactionEntry> Abstract::findByMessageId(
				memory::ConstBlockPtr messageId,
				const Filter& filter /*= Filter::ALL_TRANSACTIONS*/
			) const
			{
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
					return FilterFunctionResult::DISMISS;
					};
				return findOne(f);
			}
		}

	}
}