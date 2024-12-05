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
            DEFERRED_TIMEOUT_REVERSAL,

            /**
             * @brief Reversal of reserved decay funds after redeeming a deferred transfer.
             * 
             * This event occurs when a deferred transfer is redeemed. The reserved 
             * decay amount is returned to the original sender based 
             * on the time that has passed since the deferred transfer was created. 
             * The decay calculation uses the following formula:
             * 
             * q = 2^(-x/n)
             * 
             * Where:
             * - n = number of seconds per year
             * - x = duration in seconds since the creation of the deferred transfer
             * - q = decay factor 
             * - decayedGradido = q * originalGradidoAmount
             * 
             * Example:
             * If a deferred transfer contains 100 Gradido with a 3-month timeout, 
             * it includes an additional decay amount of 18.9635 Gradido for Decay. 
             * After one month, the remaining amount for Decay would be 12.2731 Gradido.
             * 
             * When the deferred transfer is redeemed (100 Gradido), 12.2731 Gradido 
             * would be returned to the original sender, reflecting the decay over time.
             * 
             * @see Formula for decay calculation.
             */
            DEFERRED_REDEEM_REVERSAL
        };
    }
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_TYPE_H