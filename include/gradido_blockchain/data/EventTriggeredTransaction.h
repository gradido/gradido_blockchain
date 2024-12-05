#ifndef __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H

#include "TimestampSeconds.h"
#include "EventTriggeredTransactionType.h"

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT EventTriggeredTransaction 
        {
        public:
            EventTriggeredTransaction() : mTransactionNr(0), mType(EventTriggeredTransactionType::NONE) {}
            EventTriggeredTransaction(uint64_t transactionNr, TimestampSeconds targetDate, EventTriggeredTransactionType type)
                : mTransactionNr(transactionNr), mTargetDate(targetDate), mType(type) {}

            virtual ~EventTriggeredTransaction() {}

            inline uint64_t getId() const { return mTransactionNr; }
            inline EventTriggeredTransactionType getType() const { return mType; }
            inline TimestampSeconds getTargetDate() const { return mTargetDate; }
        protected:
            //! main index
            uint64_t mTransactionNr;
            //! date on which this trigger takes effect
            TimestampSeconds mTargetDate;
            //! which type of event
            EventTriggeredTransactionType mType;
            
        };
    }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_EVENT_TRIGGERED_TRANSACTION_H