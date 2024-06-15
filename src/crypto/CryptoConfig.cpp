#include "gradido_blockchain/crypto/CryptoConfig.h"

#include "gradido_blockchain/crypto/mnemonic_german.h"
#include "gradido_blockchain/crypto/mnemonic_german2.h"
#include "gradido_blockchain/crypto/mnemonic_bip0039.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum_utility.hpp"

#include <algorithm>
#include <cstdlib>

using namespace magic_enum;

namespace CryptoConfig
{
	memory::BlockPtr g_CryptoAppSecret = nullptr;
	memory::BlockPtr g_ServerCryptoKey = nullptr;
	memory::BlockPtr g_SupportPublicKey = nullptr;

	Mnemonic g_Mnemonic_WordLists[enum_integer(MnemonicType::MAX)];

	void loadMnemonicWordLists(bool printToFile/* = false*/)
	{
		enum_for_each<MnemonicType>([&](auto val) {
			constexpr MnemonicType type = val;
			int i = enum_integer(type);
			try {
				switch (type) {
				case MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER:
					g_Mnemonic_WordLists[i].init(populate_mnemonic_german, g_mnemonic_german_original_size, g_mnemonic_german_compressed_size);
					break;
				case MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES:
					g_Mnemonic_WordLists[i].init(populate_mnemonic_german2, g_mnemonic_german2_original_size, g_mnemonic_german2_compressed_size);
					break;
				case MnemonicType::BIP0039_SORTED_ORDER:
					g_Mnemonic_WordLists[i].init(populate_mnemonic_bip0039, g_mnemonic_bip0039_original_size, g_mnemonic_bip0039_compressed_size);
					break;
                case MnemonicType::MAX:
                    return;
					//const char* what, const char* enumName, int value
				default: throw GradidoUnhandledEnum("loadMnemonicWordLists", enum_type_name<decltype(type)>().data(), enum_name(type).data());
				}
			}
			catch (MnemonicException& ex) {
				// add info and rethrow
				throw ex.setMnemonic(&g_Mnemonic_WordLists[i]);
			}
			if (printToFile) {
				std::string fileName(enum_name(type));
				std::transform(fileName.cbegin(), fileName.cend(), fileName.begin(), ::tolower);
				g_Mnemonic_WordLists[i].printToFile(fileName.data());
			}
		});
	}

	bool loadCryptoKeys(const MapEnvironmentToConfig& cfg)
	{
		// app secret for encrypt user private keys
		// TODO: encrypt with server admin key
		auto app_secret_string = cfg.getString("crypto.app_secret", "");
		if ("" != app_secret_string) {
			g_CryptoAppSecret = std::make_shared<memory::Block>(memory::Block::fromHex(app_secret_string));
		}

		// key for shorthash for comparing passwords
		auto serverKey = cfg.getString("crypto.server_key", "");
		if ("" != serverKey) {
			g_ServerCryptoKey = std::make_shared<memory::Block>(memory::Block::fromHex(serverKey));
			if (!g_ServerCryptoKey || g_ServerCryptoKey->size() != crypto_shorthash_KEYBYTES) {
				throw std::runtime_error("crypto.server_key hasn't correct size or isn't valid hex");
			}
		}

		auto supportPublicKey = cfg.getString("crypto.server_admin_public", "");
		if ("" != supportPublicKey) {
			g_SupportPublicKey = std::make_shared<memory::Block>(memory::Block::fromHex(supportPublicKey));

			if (!g_SupportPublicKey || g_SupportPublicKey->size() != ED25519_PUBLIC_KEY_SIZE) {
				throw std::runtime_error("crypto.server_admin_public hasn't correct size or isn't valid hex");
			}
		}
		return true;
	}

	void unload()
	{
		g_CryptoAppSecret.reset();
		g_ServerCryptoKey.reset();
		g_SupportPublicKey.reset();
	}

	// ####################### exception ##################################
	MissingKeyException::MissingKeyException(const char* what, const char* keyName) noexcept
		: GradidoBlockchainException(what), mKeyName(keyName)
	{

	}

	std::string MissingKeyException::getFullString() const
	{
		auto result = std::string(what());
		result += ", key name: " + mKeyName;
		return result;
	}

}
