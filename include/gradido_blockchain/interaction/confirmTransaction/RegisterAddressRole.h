#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_REGISTER_ADDRESS_ROLE_H

#include "TransactionBodyRole.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/adapter/PublicKey.h"

namespace gradido {
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT RegisterAddressRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const {
                    auto accountPublicKey = data::adapter::toConstBlockPtr(mBody->getRegisterAddress()->accountPublicKeyIndex);
                    if (!accountPublicKey || accountPublicKey->isEmpty()) return {};
                    return { data::AccountBalance(accountPublicKey, GradidoUnit::zero(), mBlockchain->getCommunityIdIndex())};
                }
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_REGISTER_ADDRESS_ROLE_H
