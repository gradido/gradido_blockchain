#include "gtest/gtest.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"

using namespace std;
using namespace chrono;
using namespace gradido::blockchain;

TEST(TimepointIntervalTest, TimepointConstructorTest)
{
	auto date = system_clock::now();
	TimepointInterval interval(date);
	EXPECT_EQ(interval.getStartDate(), date);
	EXPECT_EQ(interval.getEndDate(), date);
}

TEST(TimepointIntervalTest, Empty)
{
	TimepointInterval interval;
	EXPECT_TRUE(interval.isEmpty());

	TimepointInterval interval2(system_clock::now());
	EXPECT_FALSE(interval2.isEmpty());
}


TEST(TimepointIntervalTest, isInsideInterval)
{
	auto firstIBMRelease = Timepoint(seconds{ 366445827 }); // 1981-08-12 06:30:27
	auto firstAppleReleaseDate = Timepoint(seconds{ 443770227 }); // 1984-01-24 06:30:27
	TimepointInterval timeBetweenIBMApple(firstIBMRelease, firstAppleReleaseDate);
	auto IBM_XTRelease = Timepoint(seconds{ 415949427 }); // 1983-03-08 06:30:27
	EXPECT_TRUE(timeBetweenIBMApple.isInsideInterval(IBM_XTRelease));
	auto firstMicrosoftComputer = Timepoint(seconds{ 501323445 }); // 1985-11-20 8:30:45
	EXPECT_FALSE(timeBetweenIBMApple.isInsideInterval(firstMicrosoftComputer));
	EXPECT_TRUE(timeBetweenIBMApple.isInsideInterval(date::month(8), date::year(1981))); // first release MS-DOS
}

TEST(TimepointIntervalTest, IteratorOnlyMonths)
{
	auto startDate = Timepoint(seconds{ 1585743459 }); // 2020-04-01 12:17:39
	auto endDate = Timepoint(seconds{ 1591013859 });   // 2020-06-01 12:17:39
	TimepointInterval interval(startDate, endDate);

	const std::vector<std::pair<date::month, date::year>> expectedDates = {
		{ date::month(4), date::year(2020) },
		{ date::month(5), date::year(2020) },
		{ date::month(6), date::year(2020) }
	};

	int i = 0;
	for (auto it = interval.begin(); it != interval.end(); ++it) {
		auto date = *it;
		EXPECT_EQ(date.month(), expectedDates[i].first);
		EXPECT_EQ(date.year(), expectedDates[i].second);
		++i;
	}
}

TEST(TimepointIntervalTest, IteratorChangingYears)
{
	auto startDate = Timepoint(seconds{ 1596284259 }); // 2020-08-01 12:17:39
	auto endDate = Timepoint(seconds{ 1612181859 });   // 2021-02-01 12:17:39
	TimepointInterval interval(startDate, endDate);

	const std::vector<std::pair<date::month, date::year>> expectedDates = {
		{ date::month(8), date::year(2020) },
		{ date::month(9), date::year(2020) },
		{ date::month(10), date::year(2020) },
		{ date::month(11), date::year(2020) },
		{ date::month(12), date::year(2020) },
		{ date::month(1), date::year(2021) },
		{ date::month(2), date::year(2021) }
	};

	int i = 0;
	for (auto it = interval.begin(); it != interval.end(); ++it) {
		auto date = *it;
		EXPECT_EQ(date.month(), expectedDates[i].first);
		EXPECT_EQ(date.year(), expectedDates[i].second);
		++i;
	}
}


TEST(TimepointIntervalTest, IteratorOnlyMonthsReverse)
{
	auto startDate = Timepoint(seconds{ 1585743459 }); // 2020-04-01 12:17:39
	auto endDate   = Timepoint(seconds{ 1591013859 }); // 2020-06-01 12:17:39
	TimepointInterval interval(startDate, endDate);
	
	const std::vector<std::pair<date::month, date::year>> expectedDates = {
		{ date::month(6), date::year(2020) },
		{ date::month(5), date::year(2020) },
		{ date::month(4), date::year(2020) }
	};

	int i = 0;
	for (auto it = std::prev(interval.end()); it != interval.begin(); --it) {
		auto date = *it;
		EXPECT_EQ(date.month(), expectedDates[i].first);
		EXPECT_EQ(date.year(), expectedDates[i].second);
		++i;
	}
}

TEST(TimepointIntervalTest, IteratorChangingYearsReverse)
{
	auto startDate = Timepoint(seconds{ 1596284259 }); // 2020-08-01 12:17:39
	auto endDate = Timepoint(seconds{ 1612181859 });   // 2021-02-01 12:17:39
	TimepointInterval interval(startDate, endDate);

	const std::vector<std::pair<date::month, date::year>> expectedDates = {
		{ date::month( 2), date::year(2021) },
		{ date::month( 1), date::year(2021) },
		{ date::month(12), date::year(2020) },
		{ date::month(11), date::year(2020) },
		{ date::month(10), date::year(2020) },
		{ date::month( 9), date::year(2020) },
		{ date::month( 8), date::year(2020) }
	};

	int i = 0;
	for (auto it = std::prev(interval.end()); it != interval.begin(); --it) {
		auto date = *it;
		EXPECT_EQ(date.month(), expectedDates[i].first);
		EXPECT_EQ(date.year(), expectedDates[i].second);
		++i;
	}
}

TEST(TimepointIntervalTest, TestException)
{
	auto startDate = Timepoint(seconds{ 1612181859 }); 
	auto endDate = Timepoint(seconds{ 1596284259 });   
	EXPECT_THROW(TimepointInterval(startDate, endDate), EndDateBeforeStartDateException);
}


TEST(IterateRangeInOrderTest, AscendingIterationOverTimepointInterval)
{
	auto startDate = Timepoint(seconds{ 1585743459 }); // 2020-04-01
	auto endDate = Timepoint(seconds{ 1591013859 }); // 2020-06-01
	TimepointInterval interval(startDate, endDate);

	vector<pair<date::month, date::year>> visited;

	iterateRangeInOrder(interval.begin(), interval.end(), SearchDirection::ASC,
		[&](const date::year_month& ym) -> bool {
			visited.emplace_back(ym.month(), ym.year());
			return true;
		}
	);

	vector<pair<date::month, date::year>> expected = {
		{date::month(4), date::year(2020)},
		{date::month(5), date::year(2020)},
		{date::month(6), date::year(2020)}
	};

	ASSERT_EQ(visited, expected);
}

TEST(IterateRangeInOrderTest, DescendingIterationOverTimepointInterval)
{
	auto startDate = Timepoint(seconds{ 1585743459 }); // 2020-04-01
	auto endDate = Timepoint(seconds{ 1591013859 }); // 2020-06-01
	TimepointInterval interval(startDate, endDate);

	vector<pair<date::month, date::year>> visited;

	iterateRangeInOrder(interval.begin(), interval.end(), SearchDirection::DESC,
		[&](const date::year_month& ym) -> bool {
			visited.emplace_back(ym.month(), ym.year());
			return true;
		}
	);

	vector<pair<date::month, date::year>> expected = {
		{date::month(6), date::year(2020)},
		{date::month(5), date::year(2020)},
		{date::month(4), date::year(2020)}
	};

	ASSERT_EQ(visited, expected);
}