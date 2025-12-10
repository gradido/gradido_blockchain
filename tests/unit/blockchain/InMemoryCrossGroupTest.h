#ifndef __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_CROSS_GROUP_TEST_H
#define __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_CROSS_GROUP_TEST_H

#include "InMemoryTest.h"

class InMemoryCrossGroupTest : public InMemoryTest
{
protected:
    void SetUp() override;
    void TearDown() override;

public:
    std::shared_ptr<gradido::blockchain::Abstract> mBlockchain2;
    std::string mCommunityId2;

};

#endif //__GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_CROSS_GROUP_TEST_H