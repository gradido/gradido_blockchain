#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/interaction/createTransactionByEvent/Context.h"
#include "gradido_blockchain/interaction/createTransactionByEvent/TimeoutDeferredTransferEventRole.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	using namespace data;
	namespace interaction {
		namespace createTransactionByEvent {
			std::unique_ptr<data::GradidoTransaction> Context::run(std::shared_ptr<const TransactionTriggerEvent> transactionTriggerEvent)
			{
				std::unique_ptr<AbstractRole> role;
				auto type = transactionTriggerEvent->getType();
				switch (transactionTriggerEvent->getType()) {
				case TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL: 
					role = std::make_unique<TimeoutDeferredTransferEventRole>(transactionTriggerEvent);
					break;
				default:
					throw GradidoUnhandledEnum(
						"create transaction by this event currently not implemented",
						enum_type_name<decltype(type)>().data(),
						enum_name(type).data()
					);
				}
				return role->run(*mBlockchain);
			}
		}
	}
}