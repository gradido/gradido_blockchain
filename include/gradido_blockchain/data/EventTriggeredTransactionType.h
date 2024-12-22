#ifndef __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_TYPE_H

namespace gradido {
    namespace data {
        /**
         * @enum EventTriggeredTransactionType
         * @brief Represents the types of event-triggered transactions in the Gradido system.
         *
         * This enum defines the types of transactions that are created automatically 
         * based on certain events in the system, such as timeouts or redemptions of deferred transfers.
         */
        enum class EventTriggeredTransactionType {
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

#endif //__GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_TYPE_H