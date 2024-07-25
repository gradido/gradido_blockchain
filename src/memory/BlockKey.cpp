#include "gradido_blockchain/memory/BlockKey.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "sodium.h"

namespace memory 
{
	BlockKey::BlockKey(ConstBlockPtr block)
		: shortHash(0) 
	{
		auto serverCryptoKey = CryptoConfig::g_ServerCryptoKey;
		if (!serverCryptoKey) {
			throw CryptoConfig::MissingKeyException("in SecretKeyCryptography::createKey key is missing", "crypto.server_key");
		}
		crypto_shorthash(reinterpret_cast<unsigned char*>(&shortHash), block->data(), block->size(), *serverCryptoKey);
	}
}