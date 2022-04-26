#ifndef __GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_
#define __GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "mnemonic.h"

#include "Poco/Util/LayeredConfiguration.h"

namespace CryptoConfig
{
	enum GRADIDOBLOCKCHAIN_EXPORT Mnemonic_Types {
		MNEMONIC_GRADIDO_BOOK_GERMAN_RANDOM_ORDER,
		MNEMONIC_GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES,
		MNEMONIC_BIP0039_SORTED_ORDER,
		MNEMONIC_MAX
	};

	GRADIDOBLOCKCHAIN_EXPORT extern MemoryBin* g_CryptoAppSecret;
	GRADIDOBLOCKCHAIN_EXPORT extern MemoryBin* g_ServerCryptoKey;
	GRADIDOBLOCKCHAIN_EXPORT extern MemoryBin* g_SupportPublicKey;
	GRADIDOBLOCKCHAIN_EXPORT extern Mnemonic g_Mnemonic_WordLists[MNEMONIC_MAX];

	GRADIDOBLOCKCHAIN_EXPORT bool loadMnemonicWordLists();
	GRADIDOBLOCKCHAIN_EXPORT bool loadCryptoKeys(const Poco::Util::LayeredConfiguration& cfg);
	GRADIDOBLOCKCHAIN_EXPORT void unload();

	class MissingKeyException : GradidoBlockchainException
	{
	public:
		explicit MissingKeyException(const char* what, const char* keyName) noexcept;		

		std::string getFullString() const;
		inline const std::string& getKeyName() const { return mKeyName; }

	protected:
		std::string mKeyName;
	};

}

#endif //__GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_