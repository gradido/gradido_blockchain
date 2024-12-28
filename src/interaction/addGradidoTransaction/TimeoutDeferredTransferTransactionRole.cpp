#include "gradido_blockchain/interaction/addGradidoTransaction/TimeoutDeferredTransferTransactionRole.h"
#include "gradido_blockchain/data/GradidoTransaction.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            TimeoutDeferredTransferTransactionRole::TimeoutDeferredTransferTransactionRole(
                std::shared_ptr<data::GradidoTransaction> gradidoTransaction,
                memory::ConstBlockPtr messageId,
                Timepoint confirmedAt
            ) : TransactionBodyRole(gradidoTransaction, gradidoTransaction->getFingerprint(), confirmedAt) 
            {

            }
        }
    }
}