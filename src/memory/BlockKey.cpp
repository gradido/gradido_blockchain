#include "gradido_blockchain/memory/BlockKey.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "sodium.h"

using std::shared_ptr;

namespace memory 
{
	BlockKey::BlockKey()
		: shortHash(0)
	{

	}
	BlockKey::BlockKey(shared_ptr<const Block> block)
		: BlockKey(block->size(), block->data())
	{

	}

	BlockKey::BlockKey(size_t size, const unsigned char* data)
		: shortHash(0)
	{
		auto& serverCryptoKey = CryptoConfig::g_ServerCryptoKey;
		if (!serverCryptoKey) {
			throw CryptoConfig::MissingKeyException("in SecretKeyCryptography::createKey key is missing", "crypto.server_key");
		}
		crypto_shorthash(reinterpret_cast<unsigned char*>(&shortHash), data, size, *serverCryptoKey);
	}
}