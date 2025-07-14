
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/SealedBoxes.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
    namespace data {
        EncryptedMemo::EncryptedMemo(const std::string& memo, const AuthenticatedEncryption& communityKeyPair)
         : mKeyType(MemoKeyType::COMMUNITY_SECRET), mMemo(std::make_shared<memory::Block>(SealedBoxes::encrypt(communityKeyPair, memo)))
        {
            
        }

        EncryptedMemo::EncryptedMemo(
            const std::string& memo,
            const AuthenticatedEncryption& firstKeyPair,
            const AuthenticatedEncryption& secondKeyPair
        ) : mKeyType(MemoKeyType::SHARED_SECRET), mMemo(0)
        {
            if (firstKeyPair.hasPrivateKey()) {
                mMemo = std::make_shared<memory::Block>(firstKeyPair.encrypt(memo, secondKeyPair));
            }
            else {
                mMemo = std::make_shared<memory::Block>(secondKeyPair.encrypt(memo, firstKeyPair));
            }
        }

        std::string EncryptedMemo::decrypt(const AuthenticatedEncryption& communityKeyPair) const
        {
            return SealedBoxes::decrypt(communityKeyPair, *mMemo);
        }

        std::string EncryptedMemo::decrypt(
            const AuthenticatedEncryption& firstKeyPair,
            const AuthenticatedEncryption& secondKeyPair
        ) const 
        {
            if (firstKeyPair.hasPrivateKey()) {
                return firstKeyPair.decrypt(*mMemo, secondKeyPair).copyAsString();
            }
            else {
                return secondKeyPair.decrypt(*mMemo, firstKeyPair).copyAsString();
            }
        }
    }
}