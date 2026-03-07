#include "gradido_blockchain/types.h"

#include "date/date.h"

using date::year, date::month, date::floor, date::day, date::days, date::year_month, date::year_month_day;

static year_month_day timepointAsYearMonthDay(Timepoint date)
{
	if (date == Timepoint{}) {
		return { year(0), month(0), day(0) };
	}
	return { floor<days>(date) };
}

static year_month timepointAsYearMonth(Timepoint date)
{
	if (date == Timepoint{}) {
		return { year(0), month(0) };
	}
	auto ymd = year_month_day{ floor<days>(date) };
	return { ymd.year(), ymd.month() };
}

static Timepoint monthYearToTimepoint(const date::year_month& ym)
{
	// timepoint (std::chrono::time_point) interpret year 0 as year: -1970 we don't want that, so we use Timepoint default constructor, if year and month = 0
	if (ym.month() != date::month(0) && ym.year() != date::year(0)) {
		date::year_month_day ymd(ym.year(), ym.month(), date::day(1));
		return date::sys_days{ ymd };
	}
	return {};
}