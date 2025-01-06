#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/interaction/deserialize/AccountBalanceRole.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            AccountBalanceRole::AccountBalanceRole(const AccountBalanceMessage& accountBalance)
                : mAccountBalance(
                    std::make_shared<memory::Block>(accountBalance["pubkey"_f].value()),
                    GradidoUnit::fromInteger(accountBalance["balance"_f].value_or(0))
                )
            {
            }
        }
    }
}
