#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_TIMEOUT_DEFERRED_TRANSFER_EVENT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_TIMEOUT_DEFERRED_TRANSFER_EVENT_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransactionTriggerEvent;
	}
	namespace interaction {
		namespace createTransactionByEvent {
			class TimeoutDeferredTransferEventRole: public AbstractRole
			{
			public:
				TimeoutDeferredTransferEventRole(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent)
					: mTransactionTriggerEvent(transactionTriggerEvent) {}
				~TimeoutDeferredTransferEventRole() {}

				virtual std::unique_ptr<data::GradidoTransaction> run(const blockchain::Abstract& blockchain) const;
			protected:
				std::shared_ptr<const data::TransactionTriggerEvent> mTransactionTriggerEvent;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_TIMEOUT_DEFERRED_TRANSFER_EVENT_ROLE_H