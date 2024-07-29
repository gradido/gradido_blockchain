#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include "date/date.h"
#include "date/tz.h"
#include "gradido_blockchain/types.h"

using namespace date;
//using namespace std::chrono;
//! go N number of Months back in time, subtract day to prevent overflow
//! example: 2024-03-31 -1 month => 2024-02-29
Timepoint getPreviousNMonth(const Timepoint& startDate, int monthsAgo) {
    auto ymd = date::year_month_day(floor<days>(startDate));
    auto month = ymd.month() - date::months(monthsAgo);
		ymd -= date::months(monthsAgo);
    int subtractDays = 1;
    while(!ymd.ok()) {
      ymd = date::year_month_day(floor<days>(startDate) - days{subtractDays++});
      ymd -= date::months(monthsAgo);
    }
		return date::sys_days(ymd);
}
TEST(PreviousNMonthTest, CalculateThreeMonthsPrior) {
    auto startDate = sys_days{year{2024}/March/31};
    auto result = getPreviousNMonth(startDate, 3);
    EXPECT_EQ(result, sys_days{year{2023}/December/31});
}

TEST(PreviousNMonthTest, CalculateOneMonthsPrior) {
    auto startDate = sys_days{year{2024}/May/31};
    auto result = getPreviousNMonth(startDate, 1);
    EXPECT_EQ(result, sys_days{year{2024}/April/30});
}

TEST(PreviousNMonthTest, HandleEndOfMonth) {
    auto startDate = sys_days{year{2024}/January/31};
    auto result = getPreviousNMonth(startDate, 1);
    EXPECT_EQ(result, sys_days{year{2023}/December/31});
}

TEST(PreviousNMonthTest, HandleLeapYearMarch) {
    auto startDate = sys_days{year{2024}/March/1};
    auto result = getPreviousNMonth(startDate, 1);
    EXPECT_EQ(result, sys_days{year{2024}/February/1});
}

TEST(PreviousNMonthTest, HandleLeapYearFebruary) {
    auto startDate = sys_days{year{2024}/February/29};
    auto result = getPreviousNMonth(startDate, 12);
    EXPECT_EQ(result, sys_days{year{2023}/February/28});
}

TEST(PreviousNMonthTest, EndOfYearTransition) {
    auto startDate = sys_days{year{2024}/January/1};
    auto result = getPreviousNMonth(startDate, 1);
    EXPECT_EQ(result, sys_days{year{2023}/December/1});
}

TEST(PreviousNMonthTest, LargeNumberOfMonthsAgo) {
    auto startDate = sys_days{year{2024}/December/15};
    auto result = getPreviousNMonth(startDate, 24);
    EXPECT_EQ(result, sys_days{year{2022}/December/15});
}

TEST(PreviousNMonthTest, StartOfMonth) {
    auto startDate = sys_days{year{2024}/February/1};
    auto result = getPreviousNMonth(startDate, 1);
    EXPECT_EQ(result, sys_days{year{2024}/January/1});
}

TEST(PreviousNMonthTest, MiddleOfMonth) {
    auto startDate = sys_days{year{2024}/February/14};
    auto result = getPreviousNMonth(startDate, 3);
    EXPECT_EQ(result, sys_days{year{2023}/November/14});
}