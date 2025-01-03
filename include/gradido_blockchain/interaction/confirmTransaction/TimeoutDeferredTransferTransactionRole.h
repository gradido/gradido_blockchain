#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT TimeoutDeferredTransferTransactionRole : public TransactionBodyRole
            {
            public:
                TimeoutDeferredTransferTransactionRole(
                    std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                    memory::ConstBlockPtr messageId,
                    Timepoint confirmedAt,
                    std::shared_ptr<blockchain::Abstract> blockchain
                );
                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const;
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H