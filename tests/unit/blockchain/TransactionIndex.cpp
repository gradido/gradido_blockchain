#include "gtest/gtest.h"
#include "TransactionsIndex.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "../interaction/const.h"

#include "date/date.h"

using gradido::blockchain::CompactFilter, gradido::blockchain::SearchDirection;
using gradido::data::TransactionType;

TEST(TransactionIndex, EmptyIndexTest) {
  TransactionsIndexPublic txIdx(communityIdIndex);
  CompactFilter f;
  EXPECT_EQ(txIdx.begin(f), txIdx.end(f));
  for (auto it = txIdx.begin(f); it != txIdx.end(f); ++it) {
    // shouldn't go inside loop
    ASSERT_TRUE(false);
  }
}

TEST(TransactionIndex, LoopWithOneEntry)
{
  TransactionsIndexPublic txIdx(1);
  txIdx.addIndicesForTransaction(TransactionType::COMMUNITY_ROOT, communityIdIndex, date::year(1989), date::month(1), 1, nullptr, 0, 0);
    
  CompactFilter f;
  f.searchDirection = SearchDirection::ASC;
  auto startIt = txIdx.begin(f);
  auto endIt = txIdx.end(f);
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, 1);
  }

  f.searchDirection = SearchDirection::DESC;
  startIt = txIdx.begin(f);
  endIt = txIdx.end(f);
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, 1);
  }
}

TEST(TransactionIndex, LoopWithFiveEntries)
{
  TransactionsIndexPublic txIdx(1);
  txIdx.addIndicesForTransaction(TransactionType::COMMUNITY_ROOT, communityIdIndex, date::year(1989), date::month(1), 1, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 2, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 3, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(2), 4, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(4), 5, nullptr, 0, 0);

  CompactFilter f;
  f.searchDirection = SearchDirection::ASC;
  auto startIt = txIdx.begin(f);
  auto endIt = txIdx.end(f);
  int count = 0;
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, ++count);
  }

  f.searchDirection = SearchDirection::DESC;
  startIt = txIdx.begin(f);
  endIt = txIdx.end(f);
  count = 6;
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, --count);
  }
}

TEST(TransactionIndex, LoopWithFiveEntriesFilterRegisterAddress)
{
  TransactionsIndexPublic txIdx(1);
  txIdx.addIndicesForTransaction(TransactionType::COMMUNITY_ROOT, communityIdIndex, date::year(1989), date::month(1), 1, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 2, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 3, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(2), 4, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(4), 5, nullptr, 0, 0);

  CompactFilter f;
  f.searchDirection = SearchDirection::ASC;
  f.transactionType = TransactionType::REGISTER_ADDRESS;
  auto startIt = txIdx.begin(f);
  auto endIt = txIdx.end(f);
  int count = 1;
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, ++count);
  }

  f.searchDirection = SearchDirection::DESC;
  startIt = txIdx.begin(f);
  endIt = txIdx.end(f);
  count = 4;
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, --count);
  }
}

TEST(TransactionIndex, LoopWithFiveEntriesFilterCreation)
{
  TransactionsIndexPublic txIdx(1);
  txIdx.addIndicesForTransaction(TransactionType::COMMUNITY_ROOT, communityIdIndex, date::year(1989), date::month(1), 1, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 2, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 3, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(2), 4, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(4), 5, nullptr, 0, 0);

  CompactFilter f;
  f.searchDirection = SearchDirection::ASC;
  f.transactionType = TransactionType::CREATION;
  auto startIt = txIdx.begin(f);
  auto endIt = txIdx.end(f);
  int count = 3;
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, ++count);
  }

  f.searchDirection = SearchDirection::DESC;
  startIt = txIdx.begin(f);
  endIt = txIdx.end(f);
  count = 6;
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_EQ(*it, --count);
    EXPECT_GT(*it, 3);
  }
}

TEST(TransactionIndex, LoopWithFiveEntriesFilterIntervalHole)
{
  TransactionsIndexPublic txIdx(1);
  txIdx.addIndicesForTransaction(TransactionType::COMMUNITY_ROOT, communityIdIndex, date::year(1989), date::month(1), 1, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 2, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::REGISTER_ADDRESS, communityIdIndex, date::year(1989), date::month(1), 3, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(2), 4, nullptr, 0, 0);
  txIdx.addIndicesForTransaction(TransactionType::CREATION, communityIdIndex, date::year(1989), date::month(4), 5, nullptr, 0, 0);

  CompactFilter f;
  f.searchDirection = SearchDirection::ASC;
  f.timepointInterval = TimepointInterval({ date::year(1989), date::month(3) });
  auto startIt = txIdx.begin(f);
  auto endIt = txIdx.end(f);
  EXPECT_EQ(startIt, endIt);
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_TRUE(false);
  }

  f.searchDirection = SearchDirection::DESC;
  startIt = txIdx.begin(f);
  endIt = txIdx.end(f);
  EXPECT_EQ(startIt, endIt);
  for (auto it = startIt; it != endIt; ++it) {
    EXPECT_TRUE(false);
  }
  int zahl = 0;
}