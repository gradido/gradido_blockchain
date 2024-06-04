#ifndef __GRADIDO_BLOCKCHAIN_TEST_MODEL_PROTOPUF_H
#define __GRADIDO_BLOCKCHAIN_TEST_MODEL_PROTOPUF_H

#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include "gtest/gtest.h"

#include <vector>

class protopuf : public ::testing::Test
{

protected:
	void SetUp() override;
	void TearDown() override;

	std::vector<std::shared_ptr<KeyPairEd25519>> mTestKeyPairs;
};


#endif 