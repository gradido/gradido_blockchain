#include "gtest/gtest.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"

using namespace gradido::blockchain;

TEST(IterateRangeTest, EmptyRangeReturnsEmpty) {
    std::vector<int> data;
    Filter filter;
    // const Iterator& it, EntityType& entity
    std::vector<int> result = iterateRangeInOrderCollectFiltered<std::vector<int>::iterator, std::vector<int>>(
        data.begin(), data.end(), filter,
        [](auto&, auto&) -> FilterResult 
        { 
            return FilterResult::USE;
        }
    );
    EXPECT_TRUE(result.empty());
}

TEST(IterateRangeTest, AllElementsMatch) {
    std::vector<int> data = { 1,2,3,4,5 };
    Filter filter;
    filter.searchDirection = SearchDirection::ASC;
    auto result = iterateRangeInOrderCollectFiltered<std::vector<int>::iterator, std::vector<int>>(data.begin(), data.end(), filter, 
        [](auto& it, int& out)
        {
            out = it;
            return FilterResult::USE;
        }
    );
    EXPECT_EQ(result, data);
}

TEST(IterateRangeTest, StopSignalWorks) {
    std::vector<int> data = { 1,2,3,4,5 };
    Filter filter;
    filter.searchDirection = SearchDirection::ASC;
    int stopAfter = 3;
    auto result = iterateRangeInOrderCollectFiltered<std::vector<int>::iterator, std::vector<int>>(data.begin(), data.end(), filter, 
        [stopAfter](auto& it, int& out)
        {
            out = it;
            return (it > stopAfter) ? FilterResult::STOP : FilterResult::USE;
        }
    );
    EXPECT_EQ(result.size(), stopAfter);
}

TEST(IterateRangeTest, PaginationSkipAndLimit) {
    std::vector<int> data = { 1,2,3,4,5,6 };
    Filter filter;
    filter.searchDirection = SearchDirection::ASC;
    filter.pagination = Pagination(2, 3); // page size 2, skip until 3. page
    auto result = iterateRangeInOrderCollectFiltered<std::vector<int>::iterator, std::vector<int>>(data.begin(), data.end(), filter, 
        [](auto& it, int& out)
        {
            out = it;
            return FilterResult::USE;
        }
    );
    std::vector<int> expected = { 5,6 };
    EXPECT_EQ(result, expected);
}

TEST(IterateRangeTest, ReverseDirection) {
    std::vector<int> data = { 1,2,3 };
    Filter filter;
    filter.searchDirection = SearchDirection::DESC;
    auto result = iterateRangeInOrderCollectFiltered<std::vector<int>::iterator, std::vector<int>>(data.begin(), data.end(), filter,
        [](auto& it, int& out)
        {
            out = it;
            return FilterResult::USE;
        }
    );
    std::vector<int> expected = { 3,2,1 };
    EXPECT_EQ(result, expected);
}
