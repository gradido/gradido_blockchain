#ifndef __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_PASSPHRASE_H
#define __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_PASSPHRASE_H

#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gtest/gtest.h"

class PassphraseTest : public ::testing::Test {
protected:
	void SetUp() override;

	struct PassphraseDataSet
	{
		PassphraseDataSet(std::string _passphrases[CryptoConfig::MNEMONIC_MAX], CryptoConfig::Mnemonic_Types _type, std::string _pubkeyHex, uint16_t _wordIndices[PHRASE_WORD_COUNT])
			: mnemonicType(_type), pubkeyHex(_pubkeyHex) {
			memcpy(wordIndices, _wordIndices, PHRASE_WORD_COUNT * sizeof(uint16_t));
			for (int i = 0; i < CryptoConfig::MNEMONIC_MAX; i++) {
				passphrases[i] = _passphrases[i];
			}
		}
	
		std::string passphrases[CryptoConfig::MNEMONIC_MAX];
		CryptoConfig::Mnemonic_Types mnemonicType;
		std::string pubkeyHex;
		uint16_t wordIndices[PHRASE_WORD_COUNT];
	};

	std::vector<PassphraseDataSet> mPassphrasesForTesting;
	// void TearDown() override {}	
};

#endif //__GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_PASSPHRASE_H