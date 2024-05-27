#include "gradido_blockchain/model/protopuf/SignaturePair.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace model {
	namespace protopuf {
		
		SignaturePair::SignaturePair(const SignaturePairMessage& data)
		{
			auto mm = MemoryManager::getInstance();

			const std::vector<unsigned char>& pubkey = data["pubkey"_f].value();			
			mPubkey = mm->getMemory(pubkey.size());
			mPubkey->copyFrom(pubkey.data());

			const std::vector<unsigned char>& signature = data["signature"_f].value();
			mSignature = mm->getMemory(signature.size());
			mSignature->copyFrom(signature.data());
			
		}
		
		SignaturePair::~SignaturePair()
		{
			auto mm = MemoryManager::getInstance();
			if (mPubkey) {
				mm->releaseMemory(mPubkey);
			}
			if (mSignature) {
				mm->releaseMemory(mSignature);
			}
		}
	}
}