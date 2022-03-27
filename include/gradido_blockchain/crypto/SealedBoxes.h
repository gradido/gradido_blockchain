#ifndef __GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES
#define __GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES

#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace SealedBoxes
{
	MemoryBin* encrypt(KeyPairEd25519* keys, const std::string& message);
	std::string decrypt(KeyPairEd25519* keys, const MemoryBin* encryptedMessage);
}

#endif //__GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES