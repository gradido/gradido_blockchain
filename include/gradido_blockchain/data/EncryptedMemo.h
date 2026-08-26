#ifndef __GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H
#define __GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H

#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain_core/types/memo_key.h"

class AuthenticatedEncryption;
struct grdw_encrypted_memo;

namespace gradido {
    namespace data {
        class GRADIDOBLOCKCHAIN_EXPORT EncryptedMemo
        {
        public:
            EncryptedMemo() : mKeyType(GRDT_MEMO_KEY_PLAIN), mMemo(0) {}
            //! key type will be PLAIN, memo isn't encrypted at all
            EncryptedMemo(const std::string& memo)
                : mKeyType(GRDT_MEMO_KEY_PLAIN), mMemo(memo) {}
            //! key type will be PLAIN, memo isn't encrypted at all
            EncryptedMemo(const char* memo)
                : EncryptedMemo(std::string(memo)) {}
            EncryptedMemo(grdt_memo_key type, memory::Block&& memo)
                : mKeyType(type), mMemo(memo) {}
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
            EncryptedMemo(const grdw_encrypted_memo& memo);
            ~EncryptedMemo() {}

            inline grdt_memo_key getKeyType() const { return mKeyType; }
            inline bool isPlain() const { return mKeyType == GRDT_MEMO_KEY_PLAIN; }
            inline bool isCommunitySecret() const { return mKeyType == GRDT_MEMO_KEY_COMMUNITY_SECRET; }
            inline bool isSharedSecret() const { return mKeyType == GRDT_MEMO_KEY_SHARED_SECRET; }
            inline bool empty() const { return !mMemo; }

            inline const memory::Block& getMemo() const { return mMemo; }
            // operators
            inline bool operator==(const EncryptedMemo& other) const {
                return mKeyType == other.mKeyType && mMemo.isTheSame(other.mMemo);
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
            grdt_memo_key mKeyType;
            memory::Block mMemo;
        };

    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ENCRYPTED_MEMO_H