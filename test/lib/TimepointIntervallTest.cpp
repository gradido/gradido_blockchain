#include "gtest/gtest.h"
#include "gradido_blockchain/lib/TimepointInterval.h"

using namespace std;
using namespace chrono;

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
	int i = 0;
	for (auto& date : interval) {
		switch (i) {
		case 0:
			EXPECT_EQ(date.month(), date::month(4));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 1:
			EXPECT_EQ(date.month(), date::month(5));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 2:
			EXPECT_EQ(date.month(), date::month(6));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		}
		++i;
	}
}

TEST(TimepointIntervalTest, IteratorChangingYears)
{
	auto startDate = Timepoint(seconds{ 1596284259 }); // 2020-08-01 12:17:39
	auto endDate = Timepoint(seconds{ 1612181859 });   // 2021-02-01 12:17:39
	TimepointInterval interval(startDate, endDate);
	int i = 0;
	for (auto& date : interval) {
		switch (i) {
		case 0:
			EXPECT_EQ(date.month(), date::month(8));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 1:
			EXPECT_EQ(date.month(), date::month(9));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 2:
			EXPECT_EQ(date.month(), date::month(10));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 3:
			EXPECT_EQ(date.month(), date::month(11));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 4:
			EXPECT_EQ(date.month(), date::month(12));
			EXPECT_EQ(date.year(), date::year(2020));
			break;
		case 5:
			EXPECT_EQ(date.month(), date::month(1));
			EXPECT_EQ(date.year(), date::year(2021));
			break;
		case 6:
			EXPECT_EQ(date.month(), date::month(2));
			EXPECT_EQ(date.year(), date::year(2021));
			break;
		}
		++i;
	}
}