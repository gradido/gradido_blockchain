#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_CREATION_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_CREATION_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace createConfirmedTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT CreationTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual validate::Type getValidationType() const {
                    return TransactionBodyRole::getValidationType() | validate::Type::MONTH_RANGE;
                }
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_CREATION_TRANSACTION_ROLE_H