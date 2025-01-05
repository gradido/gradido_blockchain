#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/interaction/createConfirmedTransaction/DeferredTransferTransactionRole.h"

namespace gradido {
    using namespace data;

    namespace interaction {
        namespace createConfirmedTransaction {

            void DeferredTransferTransactionRole::runPastAddToBlockchain(
                std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) const {
                blockchain->addTransactionTriggerEvent(std::make_shared<TransactionTriggerEvent>(
                    confirmedTransaction->getId(),
                    mBody->getDeferredTransfer()->getTimeoutDuration(),
                    data::TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL
                ));
            };
        }
    }
}
