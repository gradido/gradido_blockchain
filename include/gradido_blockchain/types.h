#ifndef __GRADIDO_BLOCKCHAIN_TYPES_H
#define __GRADIDO_BLOCKCHAIN_TYPES_H

#include <chrono>
#include "date/date.h"

typedef std::chrono::time_point<std::chrono::system_clock> Timepoint;
typedef std::chrono::system_clock::duration Duration;

inline date::year_month_day timepointAsYearMonthDay(Timepoint date)
{
	if (date == Timepoint{}) {
		return { date::year(0), date::month(0), date::day(0) };
	}
	return { date::floor<date::days>(date) };
}
inline date::year_month timepointAsYearMonth(Timepoint date)
{
	if (date == Timepoint{}) {
		return { date::year(0), date::month(0) };
	}
	auto ymd = date::year_month_day{ date::floor<date::days>(date) };
	return { ymd.year(), ymd.month() };
}

#endif