#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/RedeemDeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace addGradidoTransaction {

            void RedeemDeferredTransferTransactionRole::runPastAddToBlockchain(
                std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) const {
                // check if this redeem whole deferred transfer amount
                auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
                assert(body->isRedeemDeferredTransfer());
                auto redeemDeferredTransfer = body->getRedeemDeferredTransfer();
                auto redeemAmount = redeemDeferredTransfer->getTransfer().getSender().getAmount();
                auto deferredTransferId = redeemDeferredTransfer->getDeferredTransferTransactionNr();
                auto deferredTransferEntry = blockchain->getTransactionForId(deferredTransferId);
                assert(deferredTransferEntry->getTransactionBody()->isDeferredTransfer());
                auto deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
                // if remaining amout is less than 0.01 gdd remove timeout transaction trigger event
                if (deferredTransfer->calculateUseableAmount() - redeemAmount < GradidoUnit(100ll)) {
                    blockchain->removeTransactionTriggerEvent(deferredTransferId);
                }
            };

            std::vector<data::AccountBalance> RedeemDeferredTransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transfer = mBody->getRedeemDeferredTransfer()->getTransfer();
                auto& transferAmount = transfer.getSender();
                auto deferredTransferEntry = mBlockchain->getTransactionForId(mBody->getRedeemDeferredTransfer()->getDeferredTransferTransactionNr());
                auto deferredTransferConfirmedAt = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt();
                auto deferredTransferTransaction = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
                auto& deferredTransferAmount = deferredTransferTransaction->getTransfer().getSender();
                auto decayedDeferredAmount = deferredTransferAmount.getAmount().calculateDecay(deferredTransferConfirmedAt, mConfirmedAt);
                auto change = decayedDeferredAmount - transferAmount.getAmount();
                return {
                    // sender
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount() * GradidoUnit(-1ll)),
                    // recipient
                    calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, transferAmount.getAmount()),
                    // change back to original sender of deferred transfer
                    calculateAccountBalance(deferredTransferAmount.getPublicKey(), maxTransactionNr, change)
                };

            }
        }
    }
}
