#include "gradido_blockchain/interaction/addGradidoTransaction/Context.h"

#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/CommunityRootTransactionRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/DeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/RedeemDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/TimeoutDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/TransferTransactionRole.h"
#include "gradido_blockchain/interaction/validate/Context.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	using namespace data;
	using namespace blockchain;

    namespace interaction {
        namespace addGradidoTransaction {

			std::shared_ptr<AbstractRole> Context::createRole(
				std::shared_ptr<data::GradidoTransaction> gradidoTransaction,
				memory::ConstBlockPtr messageId,
				Timepoint confirmedAt
			) const {
				// attention! work only if order in enum don't change
				static const std::array<std::function<std::shared_ptr<AbstractRole>()>, enum_integer(TransactionType::MAX_VALUE)> roleCreators = {
					[&]() { return std::make_shared<CreationTransactionRole>(gradidoTransaction, messageId, confirmedAt); },
					[&]() { return std::make_shared<TransferTransactionRole>(gradidoTransaction, messageId, confirmedAt); },
					[&]() { return nullptr; },
					[&]() { return std::make_shared<RegisterAddressRole>(gradidoTransaction, messageId, confirmedAt); },
					[&]() { return std::make_shared<DeferredTransferTransactionRole>(gradidoTransaction, messageId, confirmedAt); },
					[&]() { return std::make_shared<CommunityRootTransactionRole>(gradidoTransaction, messageId, confirmedAt); },
					[&]() { return std::make_shared<RedeemDeferredTransferTransactionRole>(gradidoTransaction, messageId, confirmedAt); },
					[&]() { return std::make_shared<TimeoutDeferredTransferTransactionRole>(gradidoTransaction, messageId, confirmedAt); }
				};

				auto type = gradidoTransaction->getTransactionBody()->getTransactionType();
				auto result = roleCreators[enum_integer(type)]();

				if (!result) {
					throw GradidoUnhandledEnum(
						"adding transaction of this type currently not implemented",
						enum_type_name<decltype(type)>().data(),
						enum_name(type).data()
					);
				}
				return result;
			}

			ResultType Context::run(std::shared_ptr<AbstractRole> role)
            {
				auto provider = mBlockchain->getProvider();
				auto communityId = mBlockchain->getCommunityId();

				interaction::validate::Context validateGradidoTransaction(*role->getGradidoTransaction());
				validateGradidoTransaction.run(interaction::validate::Type::SINGLE, communityId, provider);
				
				if (isExisting(role)) {
					return ResultType::ALREADY_EXIST;
				}

				uint64_t id = 1;
				auto lastTransaction = mBlockchain->findOne(Filter::LAST_TRANSACTION);
				if (lastTransaction) {
					auto pendingTransactions = mBlockchain->findPendingTransactionInRange({ 
						lastTransaction->getConfirmedTransaction()->getConfirmedAt(), 
						role->getConfirmedAt() 
					});
					for (auto& pendingTransaction : pendingTransactions) {
						mBlockchain->removePendingTransaction(pendingTransaction->getLinkedTransactionId());
						try {

						}
						catch (std::exception& e) {
							mBlockchain->addPendingTransaction(pendingTransaction);
							throw e;
						}
					}
					
					id = lastTransaction->getTransactionNr() + 1;
				}
				
				data::ConstConfirmedTransactionPtr lastConfirmedTransaction;
				if (lastTransaction) {
					lastConfirmedTransaction = lastTransaction->getConfirmedTransaction();
					if (role->getConfirmedAt() < lastConfirmedTransaction->getConfirmedAt().getAsTimepoint()) {
						return ResultType::INVALID_PREVIOUS_TRANSACTION_IS_YOUNGER;
					}
				}

				auto confirmedTransaction = role->createConfirmedTransaction(id, lastConfirmedTransaction, *mBlockchain);
				role->runPreValidate(confirmedTransaction, mBlockchain);

				// important! validation
				interaction::validate::Context validate(*confirmedTransaction);
				auto type = role->getValidationType();
				if (lastConfirmedTransaction) {
					type = type | interaction::validate::Type::PREVIOUS;
				}
				validate.setSenderPreviousConfirmedTransaction(lastConfirmedTransaction);
				// throw if some error occure
				validate.run(type, communityId, provider);

				addToBlockchain(confirmedTransaction);
				role->runPastAddToBlockchain(confirmedTransaction, mBlockchain);
				finalize();

				return ResultType::ADDED;
            }
        }
    }
}