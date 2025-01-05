#include "gradido_blockchain/interaction/createConfirmedTransaction/Context.h"

#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/CommunityRootTransactionRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/DeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/RedeemDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/TimeoutDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/TransferTransactionRole.h"
#include "gradido_blockchain/interaction/createTransactionByEvent/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	using namespace data;
	using namespace blockchain;

    namespace interaction {
        namespace createConfirmedTransaction {

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
				
				if (mBlockchain->isTransactionExist(role->getGradidoTransaction())) {
					return ResultType::ALREADY_EXIST;
				}

				uint64_t id = 1;
				auto lastTransaction = mBlockchain->findOne(Filter::LAST_TRANSACTION);
				data::ConstConfirmedTransactionPtr lastConfirmedTransaction;
				if (lastTransaction) {
					// check order
					lastConfirmedTransaction = lastTransaction->getConfirmedTransaction();
					if (role->getConfirmedAt() < lastConfirmedTransaction->getConfirmedAt().getAsTimepoint()) {
						return ResultType::INVALID_PREVIOUS_TRANSACTION_IS_YOUNGER;
					}
					// check if we need to add transactions triggered by events beforehand
					auto transactionTriggerEvents = mBlockchain->findTransactionTriggerEventsInRange({ 
						lastTransaction->getConfirmedTransaction()->getConfirmedAt(), 
						role->getConfirmedAt() 
					});
					for (auto& transactionTriggerEvent : transactionTriggerEvents) {
						mBlockchain->removeTransactionTriggerEvent(transactionTriggerEvent->getLinkedTransactionId());
						try {
							createTransactionByEvent::Context createTransactionByEvent(mBlockchain);
							mBlockchain->createConfirmedTransaction(createTransactionByEvent.run(transactionTriggerEvent), nullptr, transactionTriggerEvent->getTargetDate());
						}
						catch (std::exception& e) {
							mBlockchain->addTransactionTriggerEvent(transactionTriggerEvent);
							throw e;
						}
					}
					
					id = lastTransaction->getTransactionNr() + 1;
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

				mBlockchain->addToBlockchain(confirmedTransaction);
				role->runPastAddToBlockchain(confirmedTransaction, mBlockchain);

				return ResultType::ADDED;
            }
        }
    }
}