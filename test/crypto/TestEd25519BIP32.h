#ifndef __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H
#define __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H

#include "gtest/gtest.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include <list>

class TestEd25519Bip32 : public ::testing::Test
{
protected:
	void SetUp() override;
	void TearDown() override;

	struct KeyPair
	{
		KeyPair()
			: publicKey(crypto_sign_PUBLICKEYBYTES),
			privateKey(crypto_sign_SECRETKEYBYTES),
			chainCode(32) {}
		memory::Block publicKey;
		memory::Block privateKey;
		memory::Block chainCode;
	};

	std::list<KeyPair> mED25519KeyPairs;
};

#endif //__GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H