#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_EVENT_TRIGGERED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_EVENT_TRIGGERED_TRANSACTION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT EventTriggeredTransactionRole : public AbstractRole
            {
            public:
                using AbstractRole::AbstractRole;

            protected:

            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_EVENT_TRIGGERED_TRANSACTION_ROLE_H