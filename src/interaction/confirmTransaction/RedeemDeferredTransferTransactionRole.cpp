#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/confirmTransaction/RedeemDeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace confirmTransaction {

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
                auto transactionTriggerEventTargetDate =
                    deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt().getAsTimepoint()
                    + deferredTransfer->getTimeoutDuration().getAsDuration()
                ;
                // remove timeout transaction trigger event
                blockchain->removeTransactionTriggerEvent(TransactionTriggerEvent(
                    deferredTransferId,
                    transactionTriggerEventTargetDate,
                    TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL
                ));
            };

            std::vector<data::AccountBalance> RedeemDeferredTransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transfer = mBody->getRedeemDeferredTransfer()->getTransfer();
                auto& transferAmount = transfer.getSender();
                auto& communityId = transferAmount.getCommunityId();
                auto deferredTransferEntry = mBlockchain->getTransactionForId(mBody->getRedeemDeferredTransfer()->getDeferredTransferTransactionNr());
                auto deferredTransferConfirmedAt = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt();
                auto deferredTransferTransaction = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
                auto& deferredTransferAmount = deferredTransferTransaction->getTransfer().getSender();
                auto decayedDeferredAmount = deferredTransferAmount.getAmount().calculateDecay(deferredTransferConfirmedAt, mConfirmedAt);
                auto change = decayedDeferredAmount - transferAmount.getAmount();
                // if recipient and original sender are identical
                if (transfer.getRecipient()->isTheSame(deferredTransferAmount.getPublicKey())) {
                    return {
                        // sender
                        AccountBalance(transferAmount.getPublicKey(), GradidoUnit::zero(), communityId),
                        // recipient and change
                        calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, decayedDeferredAmount, communityId)
                    };
                }
                return {
                    // sender
                    AccountBalance(transferAmount.getPublicKey(), GradidoUnit::zero(), communityId),
                    // recipient
                    calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, transferAmount.getAmount(), communityId),
                    // change back to original sender of deferred transfer
                    calculateAccountBalance(deferredTransferAmount.getPublicKey(), maxTransactionNr, change, communityId)
                };

            }
        }
    }
}
