
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/SealedBoxes.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/lib/minizLib.h"
#include "gradido_blockchain_core/types/memo_key.h"

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