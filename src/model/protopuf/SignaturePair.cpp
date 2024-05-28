#include "gradido_blockchain/model/protopuf/SignaturePair.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace model {
	namespace protopuf {
		
		SignaturePair::SignaturePair(const SignaturePairMessage& data)
			: mPubkey(std::make_shared<CachedMemoryBlock>(data["pubkey"_f].value())),
			mSignature(std::make_shared<CachedMemoryBlock>(data["signature"_f].value()))
		{
		}
		SignaturePair::SignaturePair(ConstCachedMemoryBlockPtr pubkey, ConstCachedMemoryBlockPtr signature)
			: mPubkey(pubkey), mSignature(signature)
		{
		}
	}
}