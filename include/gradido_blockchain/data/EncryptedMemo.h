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
            EncryptedMemo(MemoKeyType keyType, memory::ConstBlockPtr memo)
                : mKeyType(keyType), mMemo(memo) {}

            //! with keyType == COMMUNITY_SECRET only first key will be used
            //! with keyType == SHARED_SECRET both keys are used
            //! with keyType == PLAIN no key is needed
            EncryptedMemo(
                MemoKeyType keyType,
                std::string memo,
                const AuthenticatedEncryption& firstKeyPair,
                const AuthenticatedEncryption& secondKeyPair
            );
            ~EncryptedMemo() {}

            inline MemoKeyType getKeyType() const { return mKeyType; }
            inline  memory::ConstBlockPtr getMemo() const { return mMemo; }

            //! with keyType == COMMUNITY_SECRET only first key will be used
            //! with keyType == SHARED_SECRET both keys are used
            //! with keyType == PLAIN no key is needed
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