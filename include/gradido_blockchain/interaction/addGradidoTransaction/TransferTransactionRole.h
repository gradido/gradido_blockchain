#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_TRANSFER_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT TransferTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
            protected:
            };
        }
    }
}
#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_TRANSFER_TRANSACTION_ROLE_H