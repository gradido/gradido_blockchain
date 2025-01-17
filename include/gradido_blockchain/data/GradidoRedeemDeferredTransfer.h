#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_REDEEM_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_REDEEM_DEFERRED_TRANSFER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace gradido {
    namespace data {
        class GradidoDeferredTransfer;

        /** 
         * @brief Transfer Transaction which will redeem a deferred Transfer Transaction
        */
        class GRADIDOBLOCKCHAIN_EXPORT GradidoRedeemDeferredTransfer
        {
        public:
            GradidoRedeemDeferredTransfer(uint64_t deferredTransferTransactionNr, const GradidoTransfer& transfer)
                : mDeferredTransferTransactionNr(deferredTransferTransactionNr), mTransfer(transfer) {}
            ~GradidoRedeemDeferredTransfer() {}

            inline bool operator==(const GradidoRedeemDeferredTransfer& other) const {
                return mTransfer == other.getTransfer() && mDeferredTransferTransactionNr == other.getDeferredTransferTransactionNr();
            }

            inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return mTransfer.getInvolvedAddresses(); }
            inline bool isInvolved(const memory::Block& publicKey) const { return mTransfer.isInvolved(publicKey); }
            inline memory::ConstBlockPtr getSenderPublicKey() const { return mTransfer.getSender().getPublicKey(); }
            inline memory::ConstBlockPtr getRecipientPublicKey() const { return mTransfer.getRecipient(); }

            inline uint64_t getDeferredTransferTransactionNr() const { return mDeferredTransferTransactionNr; }
            inline const GradidoTransfer& getTransfer() const { return mTransfer; }

        protected:
            uint64_t mDeferredTransferTransactionNr;
            GradidoTransfer mTransfer;            
        };
    }
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_REDEEM_DEFERRED_TRANSFER_H