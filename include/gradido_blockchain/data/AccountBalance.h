#ifndef __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H

#include "gradido_blockchain/GradidoUnit.h"
#include <string>
#include <optional>

namespace memory {
    class Block;
    using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT AccountBalance 
        {
        public:
            // empty constructor needed for swig
            AccountBalance();
            AccountBalance(memory::ConstBlockPtr publicKey, GradidoUnit balance, uint32_t communityIdIndex);
            ~AccountBalance();

            inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
            inline GradidoUnit getBalance() const { return mBalance; }
            inline uint32_t getCoinCommunityIdIndex() const { return mCoinCommunityIdIndex; }
            inline bool isTheSame(const AccountBalance& other) const;

        protected:
            memory::ConstBlockPtr mPublicKey;
            GradidoUnit mBalance;
            uint32_t mCoinCommunityIdIndex;
        };
       
        bool AccountBalance::isTheSame(const AccountBalance& other) const
        {
            if (!mPublicKey->isTheSame(other.mPublicKey)) {
                return false;
            }
            if (mBalance != other.mBalance) {
                return false;
            }
            if (mCoinCommunityIdIndex != other.mCoinCommunityIdIndex) {
                return false;
            }
            return true;
        }
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H