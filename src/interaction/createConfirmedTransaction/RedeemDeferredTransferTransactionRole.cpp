#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/RedeemDeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace createConfirmedTransaction {

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
        }
    }
}
