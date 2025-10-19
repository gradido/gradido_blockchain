#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_REGISTER_ADDRESS_ROLE_H

#include "TransactionBodyRole.h"
#include "gradido_blockchain/data/AccountBalance.h"

namespace gradido {
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT RegisterAddressRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const {
                    auto accountPublicKey = mBody->getRegisterAddress()->getAccountPublicKey();
                    if (!accountPublicKey || accountPublicKey->isEmpty()) return {};
                    return { data::AccountBalance(accountPublicKey, GradidoUnit::zero(), "")};
                }
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_REGISTER_ADDRESS_ROLE_H
