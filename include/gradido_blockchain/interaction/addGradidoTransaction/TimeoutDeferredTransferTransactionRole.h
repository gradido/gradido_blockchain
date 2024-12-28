#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT TimeoutDeferredTransferTransactionRole : public TransactionBodyRole
            {
            public:
                TimeoutDeferredTransferTransactionRole(
                    std::shared_ptr<data::GradidoTransaction> gradidoTransaction,
                    memory::ConstBlockPtr messageId,
                    Timepoint confirmedAt
                );
                ~TimeoutDeferredTransferTransactionRole() {}
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H