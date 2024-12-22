#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/interaction/deserialize/AccountBalanceRole.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            AccountBalanceRole::AccountBalanceRole(const AccountBalanceMessage& accountBalance)
                : mAccountBalance(accountBalance["balance_owner"_f].value(), accountBalance["balance"_f].value_or(0))
            {
            }
        }
    }
}