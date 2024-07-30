#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/MultithreadList.h"

#include <array>

const long double DECAY_CONST = 0.9999999780350404;// 48973201202316767079413460520837376;

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
	// 2^(x/31556952)
	// calculate for every 4. second until two years are full
	int64_t prevValue = 0;
	int64_t prevDistance = 0;
	for (int i = 1; i < 31556952 * 2; i+=4) {
		auto decaied = GradidoUnit::calculateDecay(1000000, i);
		if (prevValue) {
			ASSERT_GE(prevValue, decaied) << "previous value wasn't greater on i: " << i;
			auto distance = prevValue - decaied;
			if (prevDistance) {
				ASSERT_GE(prevDistance, distance) << "previous distance wasn't greater on i: " << i;
			}
			prevDistance = distance;
		}
		prevValue = decaied;
	}
}
