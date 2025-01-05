#include "gradido_blockchain/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/interaction/deserialize/TransactionTriggerEventRole.h"
#include "gradido_blockchain/data/Timestamp.h"

namespace gradido {
    using namespace data;
    namespace interaction {
        namespace deserialize {
            TransactionTriggerEventRole::TransactionTriggerEventRole(const TransactionTriggerEventMessage& transactionTriggerEvent)
                : mTransactionTriggerEvent()
            {
                mTransactionTriggerEvent = TransactionTriggerEvent(
                    transactionTriggerEvent["linked_transaction_nr"_f].value(),
                    TimestampRole(transactionTriggerEvent["target_date"_f].value()).data().getAsTimepoint(),
                    transactionTriggerEvent["type"_f].value()

                );
            }
        }
    }
}