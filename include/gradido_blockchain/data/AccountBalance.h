#ifndef __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H

#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/memory/Block.h"

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT AccountBalance 
        {
        public:
            // empty constructor needed for swig
            AccountBalance() {}
            AccountBalance(memory::ConstBlockPtr publicKey, GradidoUnit balance, const std::string& communityId)
                : mPublicKey(publicKey), mBalance(balance), mCommunityId(communityId) {}
            ~AccountBalance() {}

            inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
            inline GradidoUnit getBalance() const { return mBalance; }
            inline const std::string& getCommunityId() const { return mCommunityId; }
        protected:
            memory::ConstBlockPtr mPublicKey;
            GradidoUnit mBalance;
            std::string mCommunityId; // empty for home community
        };
       
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H