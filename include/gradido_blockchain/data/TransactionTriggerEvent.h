#ifndef __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H

#include "TimestampSeconds.h"
#include "TransactionTriggerEventType.h"

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT TransactionTriggerEvent 
        {
        public:
            TransactionTriggerEvent() :mLinkedTransactionNr(0), mType(TransactionTriggerEventType::NONE) {}
            TransactionTriggerEvent(
                uint64_t linkedTransactionNr,
                Timepoint targetDate,
                TransactionTriggerEventType type
            ) : mLinkedTransactionNr(linkedTransactionNr), mTargetDate(targetDate), mType(type) {}

            virtual ~TransactionTriggerEvent() {}

            inline uint64_t getLinkedTransactionId() const { return mLinkedTransactionNr; }
            inline TransactionTriggerEventType getType() const { return mType; }
            inline Timepoint getTargetDate() const { return mTargetDate; }

        protected:
            //! main index
            uint64_t mLinkedTransactionNr;
            //! date on which this trigger takes effect
            Timepoint mTargetDate;
            //! which type of event
            TransactionTriggerEventType mType;            
        };
    }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H