#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_TRIGGER_EVENT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_TRIGGER_EVENT_ROLE_H

#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransactionTriggerEvent;
	}
	namespace interaction {
		namespace serialize {

			class TransactionTriggerEventRole : public AbstractRole
			{
			public:
				TransactionTriggerEventRole(const data::TransactionTriggerEvent& transactionTriggerEvent);
				~TransactionTriggerEventRole();

				RUM_IMPLEMENTATION
				TransactionTriggerEventMessage getMessage() const;
				size_t calculateSerializedSize() const;

			protected:
				const data::TransactionTriggerEvent& mTransactionTriggerEvent;
			};

		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_TRIGGER_EVENT_ROLE_H