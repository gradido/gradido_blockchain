#ifndef __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H

#include "TimestampSeconds.h"
#include "EventTriggeredTransactionType.h"

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT EventTriggeredTransaction 
        {
        public:
            EventTriggeredTransaction() : mTransactionNr(0), mLinkedTransactionNr(0), mType(EventTriggeredTransactionType::NONE) {}
            EventTriggeredTransaction(
                uint64_t linkedTransactionNr,
                TimestampSeconds targetDate,
                EventTriggeredTransactionType type
            ) : mTransactionNr(0), mLinkedTransactionNr(linkedTransactionNr), mTargetDate(targetDate), mType(type) {}

            virtual ~EventTriggeredTransaction() {}

            inline uint64_t getId() const { return mTransactionNr; }
            inline uint64_t getLinkedTransactionId() const { return mLinkedTransactionNr; }
            inline EventTriggeredTransactionType getType() const { return mType; }
            inline TimestampSeconds getTargetDate() const { return mTargetDate; }
            inline void setTransactionNr(uint64_t transactionNr) { mTransactionNr = transactionNr; }

        protected:
            // own transaction nr
            uint64_t mTransactionNr;
            //! main index
            uint64_t mLinkedTransactionNr;
            //! date on which this trigger takes effect
            TimestampSeconds mTargetDate;
            //! which type of event
            EventTriggeredTransactionType mType;            
        };
    }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H