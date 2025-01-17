#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"
#include "gradido_blockchain/data/AccountBalance.h"

namespace gradido {
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT CommunityRootTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual validate::Type getValidationType() const { return validate::Type::SINGLE; }
                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const {
                    return {
                        data::AccountBalance(mBody->getCommunityRoot()->getAufPubkey(), GradidoUnit::zero()),
                        data::AccountBalance(mBody->getCommunityRoot()->getGmwPubkey(), GradidoUnit::zero())
                    };
                }
            protected:
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H
