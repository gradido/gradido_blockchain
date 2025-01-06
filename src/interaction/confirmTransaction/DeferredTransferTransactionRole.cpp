#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/interaction/confirmTransaction/DeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace confirmTransaction {

            void DeferredTransferTransactionRole::runPastAddToBlockchain(
                std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) const {
                Timepoint targetDate = confirmedTransaction->getConfirmedAt().getAsTimepoint()
                                        + mBody->getDeferredTransfer()->getTimeoutDuration();

                blockchain->addTransactionTriggerEvent(std::make_shared<TransactionTriggerEvent>(
                    confirmedTransaction->getId(),
                    targetDate,
                    data::TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL
                ));
            };

            std::vector<data::AccountBalance> DeferredTransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transfer = mBody->getDeferredTransfer()->getTransfer();
                auto& transferAmount = transfer.getSender();

                return {
                    // sender
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount().negated()),
                    // recipient
                    calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, transferAmount.getAmount())
                };

            }
        }
    }
}
