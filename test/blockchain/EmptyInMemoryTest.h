#ifndef __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_EMPTY_IN_MEMORY_TEST_H
#define __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_EMPTY_IN_MEMORY_TEST_H

#include "gtest/gtest.h"
#include "gradido_blockchain/blockchain/InMemory.h"

class EmptyInMemoryTest : public ::testing::Test
{
protected:
	void SetUp() override;
	void TearDown() override;

	std::shared_ptr<gradido::blockchain::InMemory> mBlockchain;
};


#endif //__GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_EMPTY_IN_MEMORY_TEST_H