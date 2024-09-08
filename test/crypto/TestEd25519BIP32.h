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

	memory::ConstBlockPtr mChainCode;
	memory::ConstBlockPtr mPublicKey;
	//memory::BlockPtr mPrivateKey;
	memory::ConstBlockPtr mSeed;
	std::shared_ptr<KeyPairEd25519> mRootKeyPair;
};

#endif //__GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H