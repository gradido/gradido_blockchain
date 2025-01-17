#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ENCRYPTED_MEMO_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ENCRYPTED_MEMO_ROLE_H

#include "Protopuf.h"

namespace gradido {
    namespace data {
        class EncryptedMemo;
    }
    namespace interaction {
        namespace deserialize {
            class EncryptedMemoRole
            {
            public:
                EncryptedMemoRole(const EncryptedMemoMessage& encryptedMemo);
                ~EncryptedMemoRole() {}
                inline operator const data::EncryptedMemo& () const { return mEncryptedMemo; }
                inline const data::EncryptedMemo& data() const { return mEncryptedMemo; }
            protected:
                data::EncryptedMemo mEncryptedMemo;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ENCRYPTED_MEMO_ROLE_H