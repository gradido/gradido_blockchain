#include <gtest/gtest.h>
#include "gradido_blockchain/lib/AccessExpireCache.h"

#include <thread>

TEST(AccessExpireCacheTest, AddAndGet) {
    AccessExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    EXPECT_TRUE(cache.get(1));
    EXPECT_EQ(cache.get(1).value(), "value1");
}

TEST(AccessExpireCacheTest, Remove) {
    AccessExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    cache.remove(1);
    EXPECT_FALSE(cache.get(1));
    EXPECT_EQ(cache.get(1), std::nullopt);
}

TEST(AccessExpireCacheTest, Timeout) {
    AccessExpireCache<int, std::string> cache(std::chrono::milliseconds(100));
    cache.add(1, "value1");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // upate timeouts only occure if add is called
    cache.add(2, "value2");
    EXPECT_FALSE(cache.get(1));
    EXPECT_EQ(cache.get(1), std::nullopt);
}

TEST(AccessExpireCacheTest, AccessResetsTimeout) {
    AccessExpireCache<int, std::string> cache(std::chrono::milliseconds(100));
    cache.add(1, "value1");
    std::this_thread::sleep_for(std::chrono::milliseconds(52));
    // upate timeouts only occure if add is called
    cache.add(2, "value2");
    EXPECT_TRUE(cache.get(1));
    EXPECT_EQ(cache.get(1).value(), "value1");
    std::this_thread::sleep_for(std::chrono::milliseconds(52));
    // upate timeouts only occure if add is called
    cache.add(3, "value3");
    EXPECT_TRUE(cache.get(1));
    EXPECT_EQ(cache.get(1).value(), "value1");
    std::this_thread::sleep_for(std::chrono::milliseconds(52));
    // upate timeouts only occure if add is called
    cache.add(4, "value4");
    EXPECT_TRUE(cache.get(1));
    EXPECT_EQ(cache.get(1).value(), "value1");
}

TEST(AccessExpireCacheTest, Size) {
    AccessExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    cache.add(2, "value2");
    EXPECT_EQ(cache.size(), 2);
    cache.remove(1);
    EXPECT_EQ(cache.size(), 1);
}

TEST(AccessExpireCacheTest, Clear) {
    AccessExpireCache<int, std::string> cache(std::chrono::milliseconds(1000));
    cache.add(1, "value1");
    cache.add(2, "value2");
    cache.clear();
    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.get(1));
    EXPECT_FALSE(cache.get(2));
}
