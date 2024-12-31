#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/TimeoutDeferredTransferTransactionRole.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            TimeoutDeferredTransferTransactionRole::TimeoutDeferredTransferTransactionRole(
                std::shared_ptr<data::GradidoTransaction> gradidoTransaction,
                memory::ConstBlockPtr messageId,
                Timepoint confirmedAt,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) : TransactionBodyRole(gradidoTransaction, gradidoTransaction->getFingerprint(), confirmedAt, blockchain) 
            {

            }

            std::vector<data::AccountBalance> TimeoutDeferredTransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto deferredTransferEntry = mBlockchain->getTransactionForId(mBody->getTimeoutDeferredTransfer()->getDeferredTransferTransactionNr());
                assert(deferredTransferEntry->getTransactionBody()->isDeferredTransfer());
                auto& deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer()->getTransfer();
                auto& deferredTransferAmount = deferredTransfer.getSender();
                auto deferredTransferConfirmedAt = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt();
                auto decayedDeferredAmount = deferredTransferAmount.getAmount().calculateDecay(deferredTransferConfirmedAt, mConfirmedAt);

                return {
                    // original sender
                    calculateAccountBalance(deferredTransferAmount.getPublicKey(), maxTransactionNr, decayedDeferredAmount),
                    // deferred transfer address 
                    calculateAccountBalance(deferredTransfer.getRecipient(), maxTransactionNr, decayedDeferredAmount * GradidoUnit(-1ll))
                };
            }
        }
    }
}