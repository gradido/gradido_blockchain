#ifndef __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_TRIGGER_EVENT_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_TRIGGER_EVENT_TYPE_H

namespace gradido {
    namespace data {
        /**
         * @enum TransactionTriggerEventType
         * @brief Represents the types event which will trigger transactions in special circumstances
         *
         * This enum defines the types of transactions that are created automatically 
         * based on certain events in the system, such as timeouts or redemptions of deferred transfers.
         */
        enum class TransactionTriggerEventType {
             /**
             * @brief Placeholder value indicating no specific event type.
             * 
             * Used as a default or uninitialized state for transactions.
             */
            NONE,

             /**
             * @brief Reversal of a deferred transfer after the timeout period expires.
             * 
             * This event occurs when a deferred transfer is not redeemed within its 
             * specified timeout period. The amount is returned to the sender, minus 
             * any decay 
             */
            DEFERRED_TIMEOUT_REVERSAL
        };
    }
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_TRIGGER_EVENT_TYPE_H