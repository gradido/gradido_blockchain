#include "main.h"
#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/MonotonicTimer.h"
#include "gradido_blockchain/lib/MultithreadList.h"

#include <array>
#include <iomanip>
#include <random>

::testing::AssertionResult IsBetweenInclusive(int64_t val, int64_t a, int64_t b)
{
  if ((val >= a) && (val <= b))
    return ::testing::AssertionSuccess();
  else
    return ::testing::AssertionFailure()
    << val << " is outside the range " << a << " to " << b;
}

TEST(GradidoUnitTest, ConstructEmpty)
{
  auto defaultConstructor = GradidoUnit();
  EXPECT_EQ(defaultConstructor, GradidoUnit::zero());
  EXPECT_EQ(defaultConstructor.getGradidoCent(), 0);
}

TEST(GradidoUnitTest, ConstructWithDouble)
{
  auto doubleConstructor = GradidoUnit(0.10212);
  EXPECT_EQ(static_cast<double>(doubleConstructor), 0.1021);
  EXPECT_EQ(doubleConstructor.getGradidoCent(), 1021);
  EXPECT_EQ(doubleConstructor.toString(), "0.1021");
}

TEST(GradidoUnitTest, ConstructWithString)
{
  auto stringConstructor = GradidoUnit::fromString("271.2817261");
  EXPECT_EQ(static_cast<double>(stringConstructor), 271.2817);
  EXPECT_EQ(stringConstructor.toString(), "271.2817");
}

TEST(GradidoUnitTest, With100Gdd14DaysDecay)
{
  auto startValue = GradidoUnit::fromGradidoCent(1000000);
  auto decayed = startValue.calculateDecay(60 * 60 * 24 * 14);
  // printf("decayed value: %s\n", decayed.toString().data());
  ASSERT_EQ(decayed.getGradidoCent(), 973781);
}

TEST(GradidoUnitTest, FromStringNegativeSubInteger)
{
  auto v = GradidoUnit::fromString("-0.0001");
  EXPECT_EQ(v.getGradidoCent(), -1);
}
