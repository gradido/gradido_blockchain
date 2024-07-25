#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_CONFIRMED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_CONFIRMED_TRANSACTION_H

#include "TimestampSeconds.h"
#include "GradidoTransaction.h"

namespace model {
	namespace protopuf {
        using ConfirmedTransactionMessage = message<
            uint64_field<"id", 1>,
            message_field<"transaction", 2, GradidoTransactionMessage>,
            message_field<"confirmed_at", 3, TimestampSecondsMessage>,
            string_field<"version_number", 4, singular, std::vector<StringCachedAlloc>>,
            bytes_field<"running_hash", 5, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
            bytes_field<"message_id", 6, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
            string_field<"account_balance", 7, singular, std::vector<StringCachedAlloc>>
        >;

        class ConfirmedTransaction
        {
        public:
            ConfirmedTransaction(const ConfirmedTransactionMessage& data);
            ConfirmedTransaction(
                uint64_t id,
                const GradidoTransaction& gradidoTransaction,
                Timepoint confirmedAt,
                const std::string& versionNumber,
                ConstMemoryBlockPtr runningHash,
                ConstMemoryBlockPtr messageId,
                const std::string& accountBalance
            );

            ~ConfirmedTransaction() {}

            inline uint64_t getId() const { return mId; }
            inline const GradidoTransaction& getGradidoTransaction() const { return mGradidoTransaction; }
            inline Timepoint getConfirmedAt() const { return mConfirmedAt.getAsTimepoint(); }
            inline const std::string& getVersionNumber() const { return mVersionNumber; }
            inline ConstMemoryBlockPtr getRunningHash() const { return mRunningHash; }
            inline ConstMemoryBlockPtr getMessageId() const { return mMessageId; }
            inline const std::string& getAccountBalance() const { return mAccountBalance; }
        protected:
            uint64_t                    mId;
            GradidoTransaction          mGradidoTransaction;
            TimestampSeconds            mConfirmedAt;
            std::string                 mVersionNumber;
            ConstMemoryBlockPtr         mRunningHash;
            ConstMemoryBlockPtr         mMessageId;
            std::string                 mAccountBalance;
        };
	}
}

#endif 