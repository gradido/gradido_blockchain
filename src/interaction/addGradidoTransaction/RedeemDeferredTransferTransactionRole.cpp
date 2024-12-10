#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/EventTriggeredTransaction.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/RedeemDeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace addGradidoTransaction {

            void RedeemDeferredTransferTransactionRole::runPastAddToBlockchain(
                std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) const {
                // blockchain->removePendingTransaction(mBody->getRedeemDeferredTransfer()->getDeferredTransferTransactionNr());
                blockchain->addEventTriggeredTransaction(std::make_shared<EventTriggeredTransaction>(
                    confirmedTransaction->getId(),
                    confirmedTransaction->getConfirmedAt(),
                    data::EventTriggeredTransactionType::DEFERRED_REDEEM_REVERSAL
                ));
            };
        }
    }
}
