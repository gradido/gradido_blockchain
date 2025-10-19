#ifndef __GRADIDO_BLOCKCHAIN_CONST_H
#define __GRADIDO_BLOCKCHAIN_CONST_H

#define GRADIDO_TRANSACTION_BODY_VERSION_STRING "3.5"
#define GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING "3.6"

// MAGIC NUMBER: max allowed Timespan between creation date of transaction and receiving date (iota milestone timestamp)
// taken 2 minutes from hedera but maybe the time isn't enough if gradido is more used
#define MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION std::chrono::seconds(120)

#define GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL std::chrono::months(3)
#define GRADIDO_DEFERRED_TRANSFER_MIN_TIMEOUT_INTERVAL std::chrono::hours(1)

// default buffer used for memo text compression, if memo is larger than 512, larger buffer will be used
#define GRADIDO_ENCRYPTED_MEMO_COMPRESSION_DEFAULT_BUFFER_BYTES 512
// skip compression if input data smaller than 120, because compressed data most likly will be bigger
#define GRADIDO_ENCRYPTED_MEMO_COMPRESSION_DEFAULT_SKIP_SIZE_BYTES 120

// passphrase word count
#define PHRASE_WORD_COUNT 24

#endif //__GRADIDO_BLOCKCHAIN_CONST_H