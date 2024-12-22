
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/SealedBoxes.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
    namespace data {
        EncryptedMemo::EncryptedMemo(
            MemoKeyType keyType,
            std::string memo,
            const AuthenticatedEncryption& firstKeyPair,
            const AuthenticatedEncryption& secondKeyPair
        ) : mKeyType(keyType) 
        {
            switch (keyType) {
            case MemoKeyType::PLAIN: 
                mMemo = std::make_shared<memory::Block>(memo);
                break;
            case MemoKeyType::SHARED_SECRET:
                if (firstKeyPair.hasPrivateKey()) {
                    mMemo = std::make_shared<memory::Block>(firstKeyPair.encrypt(memo, secondKeyPair));
                }
                else {
                    mMemo = std::make_shared<memory::Block>(secondKeyPair.encrypt(memo, firstKeyPair));
                }
                break;
            case MemoKeyType::COMMUNITY_SECRET: 
                mMemo = std::make_shared<memory::Block>(SealedBoxes::encrypt(firstKeyPair, memo));
                break;
            default: throw GradidoUnhandledEnum("not implemented", enum_type_name<decltype(keyType)>().data(), enum_name(keyType).data());
            }
        }

        std::string EncryptedMemo::decrypt(
            const AuthenticatedEncryption& firstKeyPair,
            const AuthenticatedEncryption& secondKeyPair
        ) const 
        {
            switch (mKeyType) {
            case MemoKeyType::PLAIN:
                return mMemo->copyAsString();
            case MemoKeyType::SHARED_SECRET:
                if (firstKeyPair.hasPrivateKey()) {
                    return firstKeyPair.decrypt(*mMemo, secondKeyPair).copyAsString();
                }
                else {
                    return secondKeyPair.decrypt(*mMemo, firstKeyPair).copyAsString();
                }
            case MemoKeyType::COMMUNITY_SECRET:
                return SealedBoxes::decrypt(firstKeyPair, *mMemo);
            default: throw GradidoUnhandledEnum("not implemented", enum_type_name<decltype(mKeyType)>().data(), enum_name(mKeyType).data());
            }
        }
    }
}