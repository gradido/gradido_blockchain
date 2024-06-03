#include "gradido_blockchain/data/PublicKey.h"
#include <string>

namespace gradido {
	namespace data {
		PublicKey::PublicKey(memory::ConstMemoryBlockPtr pubkeyPtr)
		{
			if (pubkeyPtr->size() != ED25519_PUBLIC_KEY_SIZE) {
				throw InvalidSizeException("invalid public key in constructor", ED25519_PUBLIC_KEY_SIZE, pubkeyPtr->size());
			}
			memcpy(publicKey.data(), *pubkeyPtr, ED25519_PUBLIC_KEY_SIZE);
		}
	}
}