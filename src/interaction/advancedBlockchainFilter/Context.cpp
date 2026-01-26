#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/Context.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/CommunityRootTransactionRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/CreationTransactionRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/DeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/RedeemDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/RegisterAddressTransactionRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/TimeoutDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/TransferTransactionRole.h"
#include "gradido_blockchain/memory/Block.h"

#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;

namespace gradido {
	using data::ConfirmedTransaction, data::TransactionBody, data::TransactionType;
	using blockchain::Filter, blockchain::FilterBuilder, blockchain::FilterResult;
	using blockchain::SearchDirection;
	using blockchain::TransactionEntry, blockchain::TransactionRelationType;

	namespace interaction {
		namespace advancedBlockchainFilter {
			shared_ptr<const TransactionEntry> Context::findRelatedTransaction(
				shared_ptr<const ConfirmedTransaction> parent,
				TransactionRelationType type
			) {
				return findRelatedTransaction(parent->getGradidoTransaction()->getTransactionBody(), type, parent->getId() - 1);
			}

			shared_ptr<const TransactionEntry> Context::findRelatedTransaction(
				shared_ptr<const TransactionBody> parentBody,
				TransactionRelationType type,
				uint64_t maxTransactionNr
			) {
				auto transactionRole = getRole(parentBody);
				if (TransactionRelationType::Previous == type) {
					return mBlockchain->getTransactionForId(maxTransactionNr);
				}
				auto publicKey = getPublicKeyForRelation(type, transactionRole);
				if (publicKey && !publicKey->isEmpty()) {
					FilterBuilder builder;
					
					return mBlockchain->findOne(builder
						.setInvolvedPublicKey(publicKey)
						.setSearchDirection(SearchDirection::DESC)
						.setMaxTransactionNr(maxTransactionNr)
						.setFilterFunction([&](const TransactionEntry& entry) -> FilterResult {
							if (entry.getConfirmedTransaction()->hasAccountBalance(*publicKey, mBlockchain->getCommunityIdIndex())) {
								return FilterResult::USE | FilterResult::STOP;
							}
							return FilterResult::DISMISS;
						})
						.build()
					);
				}
				else 
				{
					throw  GradidoUnhandledEnum(
						"role for this TransactionRelationType is currently not implemented",
						enum_type_name<decltype(type)>().data(),
						enum_name(type).data()
					);
				}
			}

			shared_ptr<AbstractTransactionRole> Context::getRole(shared_ptr<const TransactionBody> transactionBody)
			{
				// attention! work only if order in enum don't change
				static const std::array<std::function<shared_ptr<AbstractTransactionRole>()>, enum_integer(TransactionType::MAX_VALUE)> roleCreators = {
					[&]() { return std::make_shared<CreationTransactionRole>(transactionBody); },
					[&]() { return std::make_shared<TransferTransactionRole>(transactionBody); },
					[&]() { return nullptr; },
					[&]() { return std::make_shared<RegisterAddressTransactionRole>(transactionBody); },
					[&]() { return std::make_shared<DeferredTransferTransactionRole>(transactionBody); },
					[&]() { return std::make_shared<CommunityRootTransactionRole>(transactionBody); },
					[&]() { return std::make_shared<RedeemDeferredTransferTransactionRole>(transactionBody); },
					[&]() { return std::make_shared<TimeoutDeferredTransferTransactionRole>(transactionBody, mBlockchain); }
				};

				auto type = transactionBody->getTransactionType();
				auto result = roleCreators[enum_integer(type)]();

				if (!result) {
					throw GradidoUnhandledEnum(
						"role for this transaction is currently not implemented",
						enum_type_name<decltype(type)>().data(),
						enum_name(type).data()
					);
				}
				return result;
			}

			ConstBlockPtr Context::getPublicKeyForRelation(blockchain::TransactionRelationType type, shared_ptr<AbstractTransactionRole> role)
			{
				if (TransactionRelationType::RecipientPrevious == type) {
					return role->getRecipientPublicKey();
				}
				else if (TransactionRelationType::SenderPrevious == type) {
					return role->getSenderPublicKey();
				} 
				else if (TransactionRelationType::AufPrevious == type || TransactionRelationType::GmwPrevious == type) 
				{
					auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
					assert(firstTransactionEntry->getTransactionBody()->isCommunityRoot());
					auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot().value();
					if (TransactionRelationType::GmwPrevious == type) {
						return make_shared<const Block>(g_appContext->getPublicKey(communityRoot.gmwPublicKeyIndex).value());
					}
					else if (TransactionRelationType::AufPrevious == type) {
						return make_shared<const Block>(g_appContext->getPublicKey(communityRoot.aufPublicKeyIndex).value());
					}
				}
				return nullptr;
			}
		}
	}
}

