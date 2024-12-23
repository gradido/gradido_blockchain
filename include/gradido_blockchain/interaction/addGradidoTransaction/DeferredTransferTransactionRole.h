#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_DEFERRED_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_DEFERRED_TRANSFER_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"
#include "gradido_blockchain/data/EventTriggeredTransaction.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT DeferredTransferTransactionRole : public TransactionBodyRole
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

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_DEFERRED_TRANSFER_TRANSACTION_ROLE_H