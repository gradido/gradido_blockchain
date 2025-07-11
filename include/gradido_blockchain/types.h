#ifndef __GRADIDO_BLOCKCHAIN_TYPES_H
#define __GRADIDO_BLOCKCHAIN_TYPES_H

#include <chrono>
#include "date/tz.h"

typedef std::chrono::time_point<std::chrono::system_clock> Timepoint;
typedef std::chrono::system_clock::duration Duration;

inline date::year_month_day timepointAsYearMonthDay(Timepoint date)
{
	return { date::floor<date::days>(date) };
}

#endif