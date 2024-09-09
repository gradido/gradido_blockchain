#ifndef __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H
#define __GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H

#include "gtest/gtest.h"
#include "gradido_blockchain/crypto/KeyPairEd25519Ex.h"
#include <list>

class TestKeyPairEd25519Ex: public KeyPairEd25519Ex
{
public:
using KeyPairEd25519Ex::KeyPairEd25519Ex;
	inline memory::ConstBlockPtr getPrivateKey() const {return KeyPairEd25519::getPrivateKey(); }
};

class TestKeyPairEd25519: public KeyPairEd25519
{
public:
  using KeyPairEd25519::KeyPairEd25519;
	static std::shared_ptr<TestKeyPairEd25519> create(const memory::Block& seed) {
		return std::dynamic_pointer_cast<TestKeyPairEd25519>(KeyPairEd25519::create(seed));
	}
	std::shared_ptr<TestKeyPairEd25519Ex> deriveChild(uint32_t index) const {
		return std::dynamic_pointer_cast<TestKeyPairEd25519Ex>(KeyPairEd25519::deriveChild(index));
	}
	inline memory::ConstBlockPtr getPrivateKey() const {return KeyPairEd25519::getPrivateKey(); }
};




class TestEd25519Bip32 : public ::testing::Test
{
protected:
	void SetUp() override;
	void TearDown() override;

	memory::ConstBlockPtr mChainCode;
	memory::ConstBlockPtr mPublicKey;
	memory::ConstBlockPtr mSeed;
	std::shared_ptr<TestKeyPairEd25519> mRootKeyPair;
};

#endif //__GRADIDO_LOGIN_SERVER_TEST_CRYPTO_TEST_ED25519_BIP32_H