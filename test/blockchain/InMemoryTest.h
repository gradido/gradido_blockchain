#ifndef __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H
#define __GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H

#include "gtest/gtest.h"
#include "gradido_blockchain/blockchain/Abstract.h"
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
    Timepoint generateNewConfirmedAt(Timepoint createdAt);

    void createRegisterAddress();
    void createRegisterAddress(int keyPairIndexStart);
    bool createGradidoCreation(
        int recipientKeyPairIndex,
        int signerKeyPairIndex,
        GradidoUnit amount, 
        Timepoint createdAt,
        Timepoint targetDate
    );
    bool createGradidoTransfer(
        int senderKeyPairIndex,
        int recipientKeyPairIndex,
        GradidoUnit amount, 
        Timepoint createdAt
    );

    void logBlockchain();
    GradidoUnit getBalance(int keyPairIndex, Timepoint date);

    std::mt19937 gen; // seed the generator with a random value
    std::uniform_int_distribution<int> randTimeRange; // distribution for seconds between 120 and 2 days

    Timepoint mLastCreatedAt;
    Timepoint mLastConfirmedAt;
    int mKeyPairCursor;
    std::string mCommunityId;
    std::shared_ptr<gradido::blockchain::Abstract> mBlockchain;

public:
   
};

#endif //__GRADIDO_BLOCKCHAIN_TEST_BLOCKCHAIN_IN_MEMORY_TEST_H