#ifndef __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H

#include "gradido_blockchain/GradidoUnit.h"
#include <string>

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
            AccountBalance(memory::ConstBlockPtr publicKey, GradidoUnit balance, const std::string& communityId);
            ~AccountBalance();

            inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
            inline GradidoUnit getBalance() const { return mBalance; }
            inline const std::string& getCommunityId() const { return mCommunityId; }
            inline bool isTheSame(const AccountBalance& other) const;

        protected:
            memory::ConstBlockPtr mPublicKey;
            GradidoUnit mBalance;
            std::string mCommunityId; // empty for home community
        };
       
        bool AccountBalance::isTheSame(const AccountBalance& other) const
        {
            if (!mPublicKey->isTheSame(other.mPublicKey)) {
                return false;
            }
            if (mBalance != other.mBalance) {
                return false;
            }
            if (!mCommunityId.empty() || !other.mCommunityId.empty()) {
                throw GradidoNotImplementedException("comparing AccountBalances between foreign and home community");
            }
            // TODO: think about a way for getting correct community id for home community
            /*if (mCommunityId != other.mCommunityId) {
                return false;
            }*/
            return true;
        }
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ACCOUNT_BALANCE_H