#include <gtest/gtest.h>
#include "gradido_blockchain/lib/ExpireCache.h" 

#include <thread>

TEST(ExpiredCacheTest, AddAndGet) {
    ExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    EXPECT_TRUE(cache.get(1));
    EXPECT_EQ(cache.get(1).value(), "value1");
}

TEST(ExpiredCacheTest, Remove) {
    ExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    cache.remove(1);
    EXPECT_FALSE(cache.get(1));
    EXPECT_EQ(cache.get(1), std::nullopt);
}

TEST(ExpiredCacheTest, Timeout) {
    ExpireCache<int, std::string> cache(std::chrono::milliseconds(100));
    cache.add(1, "value1");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // upate timeouts only occure if add is called
    cache.add(2, "value2");
    EXPECT_FALSE(cache.get(1));
    EXPECT_EQ(cache.get(1), std::nullopt);
}

TEST(ExpiredCacheTest, Size) {
    ExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    cache.add(2, "value2");
    EXPECT_EQ(cache.size(), 2);
    cache.remove(1);
    EXPECT_EQ(cache.size(), 1);
}

TEST(ExpiredCacheTest, Clear) {
    ExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    cache.add(2, "value2");
    cache.clear();
    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.get(1));
    EXPECT_FALSE(cache.get(2));
}

