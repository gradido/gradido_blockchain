#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_REGISTER_ADDRESS_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace createConfirmedTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT RegisterAddressRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_REGISTER_ADDRESS_ROLE_H