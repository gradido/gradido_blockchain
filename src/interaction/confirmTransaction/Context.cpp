#include "gradido_blockchain/interaction/confirmTransaction/Context.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/confirmTransaction/CommunityRootTransactionRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/DeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/RedeemDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/TimeoutDeferredTransferTransactionRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/confirmTransaction/TransferTransactionRole.h"
#include "gradido_blockchain/interaction/createTransactionByEvent/Context.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"

#include "magic_enum/magic_enum.hpp"
#include "loguru/loguru.hpp"

using namespace magic_enum;
using namespace std;

namespace gradido {
	using namespace data;
	using namespace blockchain;

    namespace interaction {
        namespace confirmTransaction {

			std::shared_ptr<AbstractRole> Context::createRole(
				std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
				memory::ConstBlockPtr messageId,
				data::Timestamp confirmedAt
			) const {
				// attention! work only if order in enum don't change
				// todo: check if it is possible to use a template class for that
				const std::array<std::function<std::shared_ptr<AbstractRole>()>, enum_integer(TransactionType::MAX_VALUE)> roleCreators = {
					[&]() { return nullptr; },
					[&]() { return std::make_shared<CreationTransactionRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); },
					[&]() { return std::make_shared<TransferTransactionRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); },
					[&]() { return nullptr; },
					[&]() { return std::make_shared<RegisterAddressRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); },
					[&]() { return std::make_shared<DeferredTransferTransactionRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); },
					[&]() { return std::make_shared<CommunityRootTransactionRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); },
					[&]() { return std::make_shared<RedeemDeferredTransferTransactionRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); },
					[&]() { return std::make_shared<TimeoutDeferredTransferTransactionRole>(gradidoTransaction, messageId, confirmedAt, mBlockchain); }
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

			std::shared_ptr<const data::ConfirmedTransaction> Context::run(std::shared_ptr<AbstractRole> role)
            {				
				assert(role);

				// simple validate
				interaction::validate::Context validateGradidoTransaction(*role->getGradidoTransaction());
				validateGradidoTransaction.run(interaction::validate::Type::SINGLE, mBlockchain);

				// check if already exist
				if (mBlockchain->isTransactionExist(role->getGradidoTransaction())) {
					LOG_F(WARNING, "transaction skipped because it already exist");
					return nullptr;
				}
				while (processTransactionTrigger(role->getConfirmedAt()));
				uint64_t id = 1;
				auto lastTransaction = mBlockchain->findOne(Filter::LAST_TRANSACTION);
				data::ConstConfirmedTransactionPtr lastConfirmedTransaction;
				if (lastTransaction) {
					id = lastTransaction->getTransactionNr() + 1;
					lastConfirmedTransaction = lastTransaction->getConfirmedTransaction();
					if (role->getConfirmedAt() < lastConfirmedTransaction->getConfirmedAt()) {
						throw BlockchainOrderException("previous transaction is younger");
					}
				}
				
				auto confirmedTransaction = role->createConfirmedTransaction(id, lastConfirmedTransaction);
				role->runPreValidate(confirmedTransaction, mBlockchain);

				// important! validation
				interaction::validate::Context validate(*confirmedTransaction);
				auto type = role->getValidationType();
				if (lastConfirmedTransaction) {
					type = type | interaction::validate::Type::PREVIOUS;
				}
				validate.setSenderPreviousConfirmedTransaction(lastConfirmedTransaction);
				// throw if some error occure				

				validate.run(type, mBlockchain);
				
				return confirmedTransaction;
            }

			bool Context::processTransactionTrigger(Timepoint endDate)
			{
				auto lastTransaction = mBlockchain->findOne(Filter::LAST_TRANSACTION);
				if (!lastTransaction) {
					// no transaction, no triggers
					return false;
				}
				auto transactionTriggerEvent = mBlockchain->findNextTransactionTriggerEventInRange(
					TimepointInterval(
						lastTransaction->getConfirmedTransaction()->getConfirmedAt(),
						endDate
					)
				);
				if (!transactionTriggerEvent) {
					// no trigger, we can exit here
					return false;
				}
				mBlockchain->removeTransactionTriggerEvent(*transactionTriggerEvent);
				// TODO: maybe find a better way as recursive call
				try {
					createTransactionByEvent::Context createTransactionByEvent(mBlockchain);
					if (!mBlockchain->createAndAddConfirmedTransaction(
						createTransactionByEvent.run(transactionTriggerEvent),
						nullptr,
						transactionTriggerEvent->getTargetDate()
					)
						) {
						throw GradidoNodeInvalidDataException("Adding trigger created Transaction Failed");
					}
					return true;
				}
				catch (std::exception& e) {
					mBlockchain->addTransactionTriggerEvent(transactionTriggerEvent);
					throw e;
				}
				return false;
			}
        }
    }
}