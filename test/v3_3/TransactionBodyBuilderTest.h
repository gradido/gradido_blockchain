#ifndef __GRADIDO_BLOCKCHAIN_v3_3_TRANSACTION_BODY_BUILDER_TEST_H
#define __GRADIDO_BLOCKCHAIN_v3_3_TRANSACTION_BODY_BUILDER_TEST_H

#include "gtest/gtest.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include <array>
#include <list>
#include <memory>

class TransactionBodyBuilderTest : public ::testing::Test
{
public:

protected:
	void SetUp() override;
	void TearDown() override;

	struct KeyPair
	{
		KeyPair() : 
			publicKey(std::make_shared<memory::Block>(crypto_sign_PUBLICKEYBYTES)),
			privateKey(std::make_shared<memory::Block>(crypto_sign_SECRETKEYBYTES)) {}
		memory::BlockPtr publicKey;
		memory::BlockPtr privateKey;
	};

	std::vector<KeyPair> mED25519KeyPairs;
};

#endif //__GRADIDO_BLOCKCHAIN_v3_3_TRANSACTION_BODY_BUILDER_TEST_H