#ifndef __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H
#define __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H

#include "gtest/gtest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "../KeyPairs.h"

#include <array>
#include <random>
#include <chrono>

class InMemoryTest : public  ::testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    Timepoint generateNewCreatedAt();

    void createRegisterAddress();

    std::mt19937 gen; // seed the generator with a random value
    std::uniform_int_distribution<int> randTimeRange; // distribution for seconds between 120 and 2 days

    Timepoint mLastCreatedAt;
    int mKeyPairCursor;

    struct Account 
    {
        Account(int _pubkeyIndex)
            : pubkeyIndex(_pubkeyIndex)
        {}
        int pubkeyIndex;
        GradidoUnit balance;
        Timepoint balanceDate;
        
    };
    std::vector<Account> mAccountKeyPairIndices;

public:
   
};

#endif //__GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H