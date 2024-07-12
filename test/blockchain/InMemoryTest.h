#ifndef __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H
#define __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H

#include "gtest/gtest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"

class InMemoryTest : public ::testing::Test
{
protected:
	void SetUp() override;
	void TearDown() override;
};


#endif //__GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H