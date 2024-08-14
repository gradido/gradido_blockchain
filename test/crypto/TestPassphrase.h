#ifndef __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_PASSPHRASE_H
#define __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_PASSPHRASE_H

#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/crypto/MnemonicType.h"
#include "gradido_blockchain/const.h"
#include "gtest/gtest.h"

#include "magic_enum/magic_enum.hpp"

class PassphraseTest : public ::testing::Test {
protected:
	void SetUp() override;

	struct PassphraseDataSet
	{
		PassphraseDataSet(std::string _passphrases[magic_enum::enum_integer(MnemonicType::MAX)], MnemonicType _type, std::string _pubkeyHex, uint16_t _wordIndices[PHRASE_WORD_COUNT])
			: mnemonicType(_type), pubkeyHex(_pubkeyHex) {
			memcpy(wordIndices.data(), _wordIndices, PHRASE_WORD_COUNT * sizeof(uint16_t));
			for (int i = 0; i < magic_enum::enum_integer(MnemonicType::MAX); i++) {
				passphrases[i] = _passphrases[i];
			}
		}
	
		std::string passphrases[magic_enum::enum_integer(MnemonicType::MAX)];
		MnemonicType mnemonicType;
		std::string pubkeyHex;
		std::array<uint16_t, PHRASE_WORD_COUNT> wordIndices;
	};

	std::vector<PassphraseDataSet> mPassphrasesForTesting;
	// void TearDown() override {}	
};

#endif //__GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_PASSPHRASE_H