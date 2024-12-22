#ifndef __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H

#include "gradido_blockchain/GradidoUnit.h"
#include "BalanceOwner.h"

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT AccountBalance 
        {
        public:
            AccountBalance(BalanceOwner owner, GradidoUnit balance)
                : mOwner(owner), mBalance(balance) {}
            ~AccountBalance() {}

            inline BalanceOwner getOwner() const { return mOwner; }
            inline GradidoUnit getBalance() const { return mBalance; }
        protected:
            BalanceOwner mOwner;
            GradidoUnit mBalance;
        };
       
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H