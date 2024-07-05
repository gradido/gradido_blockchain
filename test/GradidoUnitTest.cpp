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
	// calculate for every second until two years are full
	for (int i = 0; i < 31556952 * 2; i++) {
		auto decaied = GradidoUnit::calculateDecay(1000000, std::chrono::seconds{ i });
		auto decaiedAltern = static_cast<int64_t>((100 * pow(DECAY_CONST, i)) * 10000);
		auto decaiedSeconds = static_cast<int64_t>((100 * std::exp2(static_cast<double>(-i) / 31556952.0)) * 10000);
		ASSERT_TRUE(IsBetweenInclusive(decaied, decaiedAltern - 1, decaiedAltern + 1)) << "failed for seconds: " << i;
		ASSERT_TRUE(IsBetweenInclusive(decaiedSeconds, decaiedAltern - 1, decaiedAltern + 1)) << "failed for seconds: " << i;
	}
	auto decaiedSecondsHalf = static_cast<int64_t>(100 * std::exp2(static_cast<double>(-31556952.0 / 31556952.0)) * 10000);
	printf("year: %lld\n", decaiedSecondsHalf);
}


TEST(GradidoUnitTest, TesthWithManyDifferentBalances)
{
	auto seconds = 31556952;
	auto decayFactor = GradidoUnit::calculateDecayFactor(seconds);
	auto alternativeDecayFactor = std::exp2(static_cast<double>(-seconds / 31556952.0));
	
	//for (int i = 1; i < 3884; i++) {

	/*
	for (unsigned long long i = 1; i < 4714360559524111893; i++) {
		auto value = i / 10000.0;// pow(i, 2) + log2(i);
		auto rounded = round(value * 10000) / 10000;
		auto decaied = GradidoUnit::calculateDecayFast(rounded * 10000.0, decayFactor);
		auto decaiedAltern = static_cast<int64_t>((rounded * alternativeDecayFactor) * 10000.0);
		ASSERT_TRUE(IsBetweenInclusive(decaied, decaiedAltern - 1, decaiedAltern + 100)) 
			<< "failed for i: " << i << ", that is value: " << value << ", rounded: " << rounded;
		if (i % 100) {
			printf("\r%lld/%lld", i, 4714360559524111893 - 1);
		}
	}
	*/
	std::vector<std::thread> threads;
	const int THREADS_COUNT = std::thread::hardware_concurrency();
	threads.reserve(THREADS_COUNT);
	uint64_t workBlockSize = (150844457410-1) / THREADS_COUNT;
	std::list<uint64_t> mErrors;
	std::shared_mutex mErrorsMutex;
	bool errorOccured = false;
	for (int i = 0; i < THREADS_COUNT; i++) {
		threads.push_back(
			std::thread([decayFactor, alternativeDecayFactor, &mErrors, &mErrorsMutex, &errorOccured](uint64_t start, uint64_t end) {
				for (unsigned long long i = start; i < end; i++) {
					if (errorOccured) return;
					double di = static_cast<double>(i);
					auto decaied = GradidoUnit::calculateDecayFast(i, decayFactor);
					auto decaiedAltern = static_cast<int64_t>(((di / 10000.0) * alternativeDecayFactor) * 10000.0);
					if (decaied < decaiedAltern - 1 || decaied > decaiedAltern + 60) {
						std::lock_guard _lock(mErrorsMutex);
						errorOccured = true;
						mErrors.push_back(i);
						return;
					}
				}
			},1 + i* workBlockSize, (i+1)* workBlockSize)
		);
	}
	for (auto& thread : threads) {
		thread.join();
	}
	std::lock_guard _lock(mErrorsMutex);
	for (auto i : mErrors) {
		auto value = i / 10000.0;// pow(i, 2) + log2(i);
		double di = static_cast<double>(i);
		auto rounded = round(value * 10000) / 10000;
		auto decaied = GradidoUnit::calculateDecayFast(rounded * 10000.0, decayFactor);
		auto decaiedAltern = static_cast<int64_t>((rounded * alternativeDecayFactor) * 10000.0);
		EXPECT_TRUE(IsBetweenInclusive(decaied, decaiedAltern - 1, decaiedAltern + 60))
			<< "failed for i: " << i << ", that is value: " << value << ", rounded: " << rounded;
	}
	threads.clear();
	//printf("value: %lld\n", pow(3884, 2) + log2(3884));
}

