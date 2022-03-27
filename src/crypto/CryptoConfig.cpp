#include "gradido_blockchain/crypto/CryptoConfig.h"

#include "gradido_blockchain/crypto/mnemonic_german.h"
#include "gradido_blockchain/crypto/mnemonic_german2.h"
#include "gradido_blockchain/crypto/mnemonic_bip0039.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"


namespace CryptoConfig
{
	MemoryBin* g_CryptoAppSecret = nullptr;
	MemoryBin* g_ServerCryptoKey = nullptr;

	Mnemonic g_Mnemonic_WordLists[MNEMONIC_MAX];

	bool loadMnemonicWordLists()
	{
		for (int i = 0; i < MNEMONIC_MAX; i++) {
			int iResult = 0;
			switch (i) {
			case MNEMONIC_GRADIDO_BOOK_GERMAN_RANDOM_ORDER:
				iResult = g_Mnemonic_WordLists[i].init(populate_mnemonic_german, g_mnemonic_german_original_size, g_mnemonic_german_compressed_size);
				if (iResult) {
					printf("[%s] error init german mnemonic set, error nr: %d\n", __FUNCTION__, iResult);
					return false;
				}
				g_Mnemonic_WordLists[i].printToFile("de_words.txt");
				break;
			case MNEMONIC_GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES:
				iResult = g_Mnemonic_WordLists[i].init(populate_mnemonic_german2, g_mnemonic_german2_original_size, g_mnemonic_german2_compressed_size);
				if (iResult) {
					printf("[%s] error init german mnemonic set 2, error nr: %d\n", __FUNCTION__, iResult);
					return false;
				}
				g_Mnemonic_WordLists[i].printToFile("de_words2.txt");
				break;
			case MNEMONIC_BIP0039_SORTED_ORDER:
				iResult = g_Mnemonic_WordLists[i].init(populate_mnemonic_bip0039, g_mnemonic_bip0039_original_size, g_mnemonic_bip0039_compressed_size);
				if (iResult) {
					printf("[%s] error init bip0039 mnemonic set, error nr: %d\n", __FUNCTION__, iResult);
					return false;
				}
				//g_Mnemonic_WordLists[i].printToFile("en_words.txt");
				break;
			default: printf("[%s] unknown MnemonicType\n", __FUNCTION__); return false;
			}
		}
		return true;
	}

	bool loadCryptoKeys(const Poco::Util::LayeredConfiguration& cfg)
	{
		// app secret for encrypt user private keys
		// TODO: encrypt with server admin key
		auto app_secret_string = cfg.getString("crypto.app_secret", "");
		if ("" != app_secret_string) {
			g_CryptoAppSecret = DataTypeConverter::hexToBin(app_secret_string);
		}

		// key for shorthash for comparing passwords
		auto serverKey = cfg.getString("crypto.server_key", "");
		if ("" != serverKey) {
			g_ServerCryptoKey = DataTypeConverter::hexToBin(serverKey);
			if (!g_ServerCryptoKey || g_ServerCryptoKey->size() != crypto_shorthash_KEYBYTES) {
				throw std::runtime_error("crypto.server_key hasn't correct size or isn't valid hex");
			}
		}

		auto supportPublicKey = cfg.getString("crypto.server_admin_public");
		if ("" != supportPublicKey) {
			g_SupportPublicKey = DataTypeConverter::hexToBin(supportPublicKey);
			if (!g_SupportPublicKey || g_SupportPublicKey->size() != KeyPairEd25519::getPublicKeySize()) {
				throw std::runtime_error("crypto.server_admin_public hasn't correct size or isn't valid hex");
			}
		}
		return true;
	}

	void unload()
	{
		auto mm = MemoryManager::getInstance();
		if (g_CryptoAppSecret) {
			mm->releaseMemory(g_CryptoAppSecret);
			g_CryptoAppSecret = nullptr;
		}

		if (g_ServerCryptoKey) {
			mm->releaseMemory(g_ServerCryptoKey);
			g_ServerCryptoKey = nullptr;
		}

		if (g_SupportPublicKey) {
			mm->releaseMemory(g_SupportPublicKey);
			g_SupportPublicKey = nullptr;
		}
	}

	// ####################### exception ##################################
	MissingKeyException::MissingKeyException(const char* what, const char* keyName) noexcept
		: GradidoBlockchainException(what), mKeyName(keyName)
	{

	}

	std::string MissingKeyException::getFullString() const 
	{
		auto result = std::string(what());
		result += ", key name: %s";
		return result;
	}

}