#ifndef __GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_
#define __GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_

#include "gradido_blockchain/MemoryManager.h"
#include "mnemonic.h"

namespace CryptoConfig
{
	enum Mnemonic_Types {
		MNEMONIC_GRADIDO_BOOK_GERMAN_RANDOM_ORDER,
		MNEMONIC_GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES,
		MNEMONIC_BIP0039_SORTED_ORDER,
		MNEMONIC_MAX
	};

	extern MemoryBin* g_CryptoAppSecret;
	extern MemoryBin* g_ServerCryptoKey;
	extern Mnemonic g_Mnemonic_WordLists[MNEMONIC_MAX];

	bool loadMnemonicWordLists();
}

#endif //__GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_