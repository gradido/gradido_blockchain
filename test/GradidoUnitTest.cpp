#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/Profiler.h"

#include <array>

auto decayConst = 0.99999997803504048973201202316767079413460520837376;

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
	EXPECT_EQ(doubleConstructor.toString(), "0.1021");
}

TEST(GradidoUnitTest, ConstructWithString)
{
	auto stringConstructor = GradidoUnit("271.2817261");
	EXPECT_EQ(static_cast<double>(stringConstructor), 271.2817);
	EXPECT_EQ(stringConstructor.toString(), "271.2817");
}

TEST(GradidoUnitTest, TestWithManyDifferentDuration)
{
	// calculate for every second until a year is full
	for (int i = 0; i < 31556952; i++) {
		auto decaied = GradidoUnit::calculateDecay(1000000, std::chrono::seconds{ i });
		auto decaiedAltern = static_cast<int64_t>((100 * pow(decayConst, i)) * 10000);
		ASSERT_TRUE(IsBetweenInclusive(decaied, decaiedAltern - 1, decaiedAltern + 1)) << "failed for seconds: " << i;
	}
}
