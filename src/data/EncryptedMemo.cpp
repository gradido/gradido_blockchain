
#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/SealedBoxes.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/lib/minizLib.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain_core/types/memo_key.h"
#include "gradido_blockchain_core/data/wire/basic_types.h"

namespace gradido {
    namespace data {
        EncryptedMemo::EncryptedMemo(const std::string& memo, const AuthenticatedEncryption& communityKeyPair)
         : mKeyType(GRDT_MEMO_KEY_COMMUNITY_SECRET), mMemo(0)
        {
            auto compressedMemo = compress(memo);
            mMemo = SealedBoxes::encrypt(communityKeyPair, compressedMemo.copyAsString());
        }

        EncryptedMemo::EncryptedMemo(
            const std::string& memo,
            const AuthenticatedEncryption& firstKeyPair,
            const AuthenticatedEncryption& secondKeyPair
        ) : mKeyType(GRDT_MEMO_KEY_SHARED_SECRET), mMemo(0)
        {
            auto compressedMemo = compress(memo);
            if (firstKeyPair.hasPrivateKey()) {
                mMemo = firstKeyPair.encrypt(compressedMemo, secondKeyPair);
            }
            else {
                mMemo = secondKeyPair.encrypt(compressedMemo, firstKeyPair);
            }
        }

        EncryptedMemo::EncryptedMemo(const grdw_encrypted_memo& memo)
            : mKeyType(memo.type), mMemo(memo.memo.size, memo.memo.data)
        {

        }

        std::string EncryptedMemo::decrypt(const AuthenticatedEncryption& communityKeyPair) const
        {
            return decompress(SealedBoxes::decrypt(communityKeyPair, mMemo)).copyAsString();
        }

        std::string EncryptedMemo::decrypt(
            const AuthenticatedEncryption& firstKeyPair,
            const AuthenticatedEncryption& secondKeyPair
        ) const 
        {
            memory::Block compressedMemo(0);
            if (firstKeyPair.hasPrivateKey()) {
                compressedMemo = firstKeyPair.decrypt(mMemo, secondKeyPair);
            }
            else {
                compressedMemo = secondKeyPair.decrypt(mMemo, firstKeyPair);
            }
            return decompress(compressedMemo).copyAsString();
        }
    }
}