#pragma once

#include "gradido_blockchain/blockchain/TransactionsIndex.h"

#include "gtest/gtest.h"

class TransactionsIndexPublic : public gradido::blockchain::TransactionsIndex 
{
public:
    using TransactionsIndex::TransactionsIndex;
    using TransactionsIndex::addIndicesForTransaction;
};
