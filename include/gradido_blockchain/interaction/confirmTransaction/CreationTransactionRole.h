#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_CREATION_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_CREATION_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"

namespace gradido {
    namespace data::compact {
      struct CommunityRootTx;
    }
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT CreationTransactionRole : public TransactionBodyRole
            {
            public:
                using TransactionBodyRole::TransactionBodyRole;
                virtual validate::Type getValidationType() const {
                    return TransactionBodyRole::getValidationType() | validate::Type::MONTH_RANGE;
                }
                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const;
            private:
                data::AccountBalance accountBalanceFromPublicKeyIndex(
                  uint64_t maxTransactionNr,
                  uint32_t publicKeyIndex, 
                  data::compact::CommunityRootTx& startTransaction
                ) const;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_CREATION_TRANSACTION_ROLE_H