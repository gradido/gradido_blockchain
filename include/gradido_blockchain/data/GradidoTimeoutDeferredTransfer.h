#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace gradido {
    namespace data {
        class GradidoDeferredTransfer;

        /**
         * @brief Transfer Transaction which will redeem a deferred Transfer Transaction
        */
        class GRADIDOBLOCKCHAIN_EXPORT GradidoTimeoutDeferredTransfer
        {
        public:
            GradidoTimeoutDeferredTransfer(uint64_t deferredTransferTransactionNr)
                : mDeferredTransferTransactionNr(deferredTransferTransactionNr) {}
            ~GradidoTimeoutDeferredTransfer() {}

            inline bool operator==(const GradidoTimeoutDeferredTransfer& other) const {
                return mDeferredTransferTransactionNr == other.getDeferredTransferTransactionNr();
            }

            inline uint64_t getDeferredTransferTransactionNr() const { return mDeferredTransferTransactionNr; }

        protected:
            uint64_t mDeferredTransferTransactionNr;
        };
    }
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_H