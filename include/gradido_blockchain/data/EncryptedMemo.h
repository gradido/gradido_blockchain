#ifndef __GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H
#define __GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H

#include "gradido_blockchain/GradidoUnit.h"
#include "MemoKeyType.h"

class AuthenticatedEncryption;

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT EncryptedMemo
        {
        public:
            EncryptedMemo() : mKeyType(MemoKeyType::PLAIN) {}
            //! key type will be PLAIN, memo isn't encrypted at all
            EncryptedMemo(std::string_view memo)
                : mKeyType(MemoKeyType::PLAIN), mMemo(std::make_shared<memory::Block>(memo)) {}
            EncryptedMemo(MemoKeyType type, memory::ConstBlockPtr memo)
                : mKeyType(type), mMemo(memo) {}

            //! key type will be COMMUNITY_SECRET, memo is encrypted with community server key and can be seen by all community server user
            EncryptedMemo(std::string memo, const AuthenticatedEncryption& communityKeyPair);
            //! key type will be SHARED_SECRET, memo is encrypted with shared secret calculated from sender public key and recipient private key or vice versa
            EncryptedMemo(
                std::string memo,
                const AuthenticatedEncryption& firstKeyPair,
                const AuthenticatedEncryption& secondKeyPair
            );
            ~EncryptedMemo() {}

            inline MemoKeyType getKeyType() const { return mKeyType; }
            inline  memory::ConstBlockPtr getMemo() const { return mMemo; }
            inline bool operator==(const EncryptedMemo& other) const {
                return mKeyType == other.mKeyType && mMemo->isTheSame(other.mMemo);
            }

            //! with keyType == COMMUNITY_SECRET 
            std::string decrypt(const AuthenticatedEncryption& communityKeyPair) const;

            //! with keyType == SHARED_SECRET 
            std::string decrypt(
                const AuthenticatedEncryption& firstKeyPair,
                const AuthenticatedEncryption& secondKeyPair
            ) const;
            
        protected:
            MemoKeyType mKeyType;
            memory::ConstBlockPtr mMemo;
        };

    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H