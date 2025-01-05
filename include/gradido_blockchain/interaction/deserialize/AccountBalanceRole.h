#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ACCOUNT_BALANCE_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ACCOUNT_BALANCE_ROLE_H

#include "Protopuf.h"

namespace gradido {
    namespace data {
        class AccountBalance;
    }
    namespace interaction {
        namespace deserialize {
            class AccountBalanceRole
            {
            public:
                AccountBalanceRole(const AccountBalanceMessage& accountBalance);
                inline operator const data::AccountBalance& () const { return mAccountBalance; }
                inline const data::AccountBalance& data() const { return mAccountBalance; }
            protected:
                data::AccountBalance mAccountBalance;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ACCOUNT_BALANCE_ROLE_H