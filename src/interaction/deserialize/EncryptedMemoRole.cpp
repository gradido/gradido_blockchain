#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/interaction/deserialize/EncryptedMemoRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            EncryptedMemoRole::EncryptedMemoRole(const EncryptedMemoMessage& encryptedMemo)
            {
                if (!encryptedMemo["memo"_f].has_value()) {
                    throw MissingMemberException("missing member on deserialize EncryptedMemo", "memo");
                }
                mEncryptedMemo = data::EncryptedMemo(encryptedMemo["type"_f].value(), std::make_shared<memory::Block>(encryptedMemo["memo"_f].value()));
            }
        }
    }
}