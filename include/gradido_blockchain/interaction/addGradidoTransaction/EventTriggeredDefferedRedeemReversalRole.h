#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_EVENT_TRIGGERED_DEFFERED_REDEEM_REVERSAL_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_EVENT_TRIGGERED_DEFFERED_REDEEM_REVERSAL_ROLE_H

#include "EventTriggeredTransactionRole.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT EventTriggeredDefferedRedeemReversalRole : public EventTriggeredTransactionRole
            {
            public:
                using EventTriggeredTransactionRole::EventTriggeredTransactionRole;
            protected:

            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_EVENT_TRIGGERED_DEFFERED_REDEEM_REVERSAL_ROLE_H