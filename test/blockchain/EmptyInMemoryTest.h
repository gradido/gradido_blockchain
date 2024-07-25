#ifndef __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_EMPTY_IN_MEMORY_TEST_H
#define __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_EMPTY_IN_MEMORY_TEST_H

#include "gtest/gtest.h"

class EmptyInMemoryTest : public ::testing::Test
{
protected:
	void SetUp() override;
	void TearDown() override;
};


#endif //__GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_EMPTY_IN_MEMORY_TEST_H