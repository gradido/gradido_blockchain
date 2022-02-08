#include "gradido_blockchain/crypto/CryptoConfig.h"

#include "gradido_blockchain/crypto/mnemonic_german.h"
#include "gradido_blockchain/crypto/mnemonic_german2.h"
#include "gradido_blockchain/crypto/mnemonic_bip0039.h"

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
}