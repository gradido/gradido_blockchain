#ifndef __GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_
#define __GRADIDO_BLOCKCHAIN_LIB_CRYPTO_CONFIG_

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/MapEnvironmentToConfig.h"
#include "gradido_blockchain/memory/Block.h"
#include "mnemonic.h"
#include "MnemonicType.h"

#include "magic_enum/magic_enum.hpp"

using namespace memory;

// TODO: change to class to make it write protected!
namespace CryptoConfig
{
	GRADIDOBLOCKCHAIN_EXPORT extern memory::BlockPtr g_CryptoAppSecret;
	GRADIDOBLOCKCHAIN_EXPORT extern memory::BlockPtr g_ServerCryptoKey;
	GRADIDOBLOCKCHAIN_EXPORT extern memory::BlockPtr g_SupportPublicKey;
	GRADIDOBLOCKCHAIN_EXPORT extern Mnemonic g_Mnemonic_WordLists[magic_enum::enum_integer(MnemonicType::MAX)];

	// throw exception on error
	//!
	//! \param printToFile if true, print every word list in a separat txt file for debugging
	GRADIDOBLOCKCHAIN_EXPORT void loadMnemonicWordLists(bool printToFile = false);
	GRADIDOBLOCKCHAIN_EXPORT bool loadCryptoKeys(const MapEnvironmentToConfig& cfg);
	GRADIDOBLOCKCHAIN_EXPORT void unload();

	class GRADIDOBLOCKCHAIN_EXPORT MissingKeyException : GradidoBlockchainException
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
