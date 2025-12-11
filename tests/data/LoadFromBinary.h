#ifndef __GRADIDO_BLOCKCHAIN_DATA_TEST_LOAD_FROM_BINARY_H
#define __GRADIDO_BLOCKCHAIN_DATA_TEST_LOAD_FROM_BINARY_H

#include "gtest/gtest.h"

class LoadFromBinary : public  ::testing::Test
{
public:
protected:
	void SetUp() override;
	void TearDown() override;
};

#endif //__GRADIDO_BLOCKCHAIN_DATA_TEST_LOAD_FROM_BINARY_H