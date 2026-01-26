#ifndef __GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H
#define __GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H

#include "gradido_blockchain/GradidoUnit.h"
#include "MemoKeyType.h"
#include "gradido_blockchain/memory/Block.h"

class AuthenticatedEncryption;

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT EncryptedMemo
        {
        public:
            EncryptedMemo() : mKeyType(MemoKeyType::PLAIN), mMemo(0) {}
            //! key type will be PLAIN, memo isn't encrypted at all
            EncryptedMemo(const std::string& memo)
                : mKeyType(MemoKeyType::PLAIN), mMemo(std::make_shared<memory::Block>(memo)) {}
            //! key type will be PLAIN, memo isn't encrypted at all
            EncryptedMemo(const char* memo)
                : EncryptedMemo(std::string(memo)) {}
            EncryptedMemo(MemoKeyType type, memory::Block&& memo)
                : mKeyType(type), mMemo(std::make_shared<memory::Block>(std::move(memo))) {}
            //! key type will be COMMUNITY_SECRET, memo is encrypted with community server key and can be seen by all community server user
            EncryptedMemo(const std::string& memo, const AuthenticatedEncryption& communityKeyPair);
            EncryptedMemo(const char* memo, const AuthenticatedEncryption& communityKeyPair)
                : EncryptedMemo(std::string(memo), communityKeyPair) {}
            //! key type will be SHARED_SECRET, memo is encrypted with shared secret calculated from sender public key and recipient private key or vice versa
            EncryptedMemo(
                const std::string& memo,
                const AuthenticatedEncryption& firstKeyPair,
                const AuthenticatedEncryption& secondKeyPair
            );
            EncryptedMemo(const char* memo, const AuthenticatedEncryption& firstKeyPair, const AuthenticatedEncryption& secondKeyPair)
                : EncryptedMemo(std::string(memo), firstKeyPair, secondKeyPair) {}
            // move
            EncryptedMemo(EncryptedMemo&& other) noexcept : mKeyType(other.mKeyType), mMemo(std::move(other.mMemo)) {}
            // copy 
            EncryptedMemo(const EncryptedMemo& other) : mKeyType(other.mKeyType), mMemo(other.mMemo) {}
            ~EncryptedMemo() {}

            inline MemoKeyType getKeyType() const { return mKeyType; }
            inline bool isPlain() const { return mKeyType == MemoKeyType::PLAIN; }
            inline bool isCommunitySecret() const { return mKeyType == MemoKeyType::COMMUNITY_SECRET; }
            inline bool isSharedSecret() const { return mKeyType == MemoKeyType::SHARED_SECRET; }

            inline const memory::Block& getMemo() const { return *mMemo; }
            inline memory::ConstBlockPtr getMemoPtr() const { return mMemo; }
            // operators
            inline bool operator==(const EncryptedMemo& other) const {
                return mKeyType == other.mKeyType && mMemo->isTheSame(other.mMemo);
            }            
            // move
            EncryptedMemo& operator=(EncryptedMemo&& other) noexcept {
                mKeyType = other.mKeyType;
                mMemo = std::move(other.mMemo);
                return *this;
            }
            // copy
            EncryptedMemo& operator=(const EncryptedMemo& other) {
                mKeyType = other.mKeyType;
                mMemo = other.mMemo;
                return *this;
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