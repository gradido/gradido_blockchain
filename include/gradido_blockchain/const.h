#ifndef __GRADIDO_BLOCKCHAIN_CONST_H
#define __GRADIDO_BLOCKCHAIN_CONST_H

#define GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING "3.3"
#define GRADIDO_CONFIRMED_TRANSACTION_V3_3_VERSION_STRING "3.3"

// MAGIC NUMBER: max allowed Timespan between creation date of transaction and receiving date (iota milestone timestamp)
// taken 2 minutes from hedera but maybe the time isn't enough if gradido is more used
#define MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION std::chrono::seconds(120)

#define GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL std::chrono::months(3)

#endif //__GRADIDO_BLOCKCHAIN_CONST_H