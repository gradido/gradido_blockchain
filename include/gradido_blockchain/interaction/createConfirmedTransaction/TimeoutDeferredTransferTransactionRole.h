#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace createConfirmedTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT TimeoutDeferredTransferTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual void runPastAddToBlockchain(
                    std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                    std::shared_ptr<blockchain::Abstract> blockchain
                ) const;
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H