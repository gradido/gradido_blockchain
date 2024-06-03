#include "gradido_blockchain/model/protopuf/SignaturePair.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace model {
	namespace protopuf {
		
		SignaturePair::SignaturePair(const SignaturePairMessage& data)
			: mPubkey(std::make_shared<memory::Block>(data["pubkey"_f].value())),
			mSignature(std::make_shared<memory::Block>(data["signature"_f].value()))
		{
		}
		SignaturePair::SignaturePair(ConstMemoryBlockPtr pubkey, ConstMemoryBlockPtr signature)
			: mPubkey(pubkey), mSignature(signature)
		{
		}
	}
}