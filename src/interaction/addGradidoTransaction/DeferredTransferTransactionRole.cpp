#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/EventTriggeredTransaction.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/DeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace addGradidoTransaction {

            void DeferredTransferTransactionRole::runPastAddToBlockchain(
                std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) const {
                blockchain->addPendingTransaction(std::make_shared<EventTriggeredTransaction>(
                    confirmedTransaction->getId(),
                    mBody->getDeferredTransfer()->getTimeout(),
                    data::EventTriggeredTransactionType::DEFERRED_TIMEOUT_REVERSAL
                ));
            };
        }
    }
}
