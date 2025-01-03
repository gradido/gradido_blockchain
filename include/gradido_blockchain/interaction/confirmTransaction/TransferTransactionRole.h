#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TRANSFER_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT TransferTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const;
            };
        }
    }
}
#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TRANSFER_TRANSACTION_ROLE_H