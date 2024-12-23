#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT CommunityRootTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual validate::Type getValidationType() const { return validate::Type::SINGLE; }
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H