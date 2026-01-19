#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/confirmTransaction/TimeoutDeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;
    namespace interaction {
        namespace confirmTransaction {
            TimeoutDeferredTransferTransactionRole::TimeoutDeferredTransferTransactionRole(
                std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                const data::LedgerAnchor& ledgerAnchor,
                Timestamp confirmedAt,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) : TransactionBodyRole(gradidoTransaction, ledgerAnchor, confirmedAt, blockchain)
            {

            }

            std::vector<data::AccountBalance> TimeoutDeferredTransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto deferredTransferEntry = mBlockchain->getTransactionForId(mBody->getTimeoutDeferredTransfer()->getDeferredTransferTransactionNr());
                assert(deferredTransferEntry->getTransactionBody()->isDeferredTransfer());
                auto& deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer()->getTransfer();
                auto& deferredTransferAmount = deferredTransfer.getSender();
                auto coinCommunityIdIndex = deferredTransferAmount.getCoinCommunityIdIndex();
                auto deferredTransferConfirmedAt = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt();
                auto decayedDeferredAmount = deferredTransferAmount.getAmount().calculateDecay(deferredTransferConfirmedAt, mConfirmedAt);

                return {
                    // original sender
                    calculateAccountBalance(deferredTransferAmount.getPublicKey(), maxTransactionNr, decayedDeferredAmount, coinCommunityIdIndex),
                    // deferred transfer address 
                    AccountBalance(deferredTransfer.getRecipient(), GradidoUnit::zero(), coinCommunityIdIndex)
                };
            }
        }
    }
}