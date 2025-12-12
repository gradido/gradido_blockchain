#include "gradido_blockchain/data/SignaturePair.h"
#include "gradido_blockchain/memory/Block.h"
#include "sodium.h"

using memory::Block, memory::ConstBlockPtr;

namespace gradido {
	namespace data {

		SignaturePair::SignaturePair()
			: mHash(0)
		{

		}

		SignaturePair::SignaturePair(ConstBlockPtr pubkeyPtr, ConstBlockPtr signaturePtr)
			: mPublicKey(pubkeyPtr), mSignature(signaturePtr), mHash(calculateHash())
		{

		}

		SignaturePair::~SignaturePair()
		{

		}

		int64_t SignaturePair::calculateHash() const
		{
			int64_t shortHash = 0;
			assert(crypto_shorthash_BYTES == sizeof(shortHash));

			// take every second byte from public key
			Block key(crypto_shorthash_KEYBYTES);
			assert(mPublicKey->size() >= key.size() * 2);
			for (int i = 0; i < crypto_shorthash_KEYBYTES; i++) {
				key[i] = mPublicKey->data()[i * 2];
			}
			crypto_shorthash(reinterpret_cast<unsigned char*>(&shortHash), *mSignature, mSignature->size(), key);
			return shortHash;
		}
	}
}