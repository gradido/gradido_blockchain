#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_TRIGGER_EVENT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_TRIGGER_EVENT_ROLE_H

#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "Protopuf.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class TransactionTriggerEventRole
            {
            public:
                TransactionTriggerEventRole(const TransactionTriggerEventMessage& transactionTriggerEvent);
                ~TransactionTriggerEventRole() {};
                inline operator const data::TransactionTriggerEvent& () const { return mTransactionTriggerEvent; }
                inline const data::TransactionTriggerEvent& data() const { return mTransactionTriggerEvent; }
            protected:
                data::TransactionTriggerEvent mTransactionTriggerEvent;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_TRIGGER_EVENT_ROLE_H