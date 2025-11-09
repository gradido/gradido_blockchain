#ifndef __GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H
#define __GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H

#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "date/date.h"

class TimepointInterval
{
public:
	TimepointInterval() {}

	TimepointInterval(Timepoint startDate, Timepoint endDate)
		: mStartDate(startDate), mEndDate(endDate) {
			if(startDate > endDate) {
				throw EndDateBeforeStartDateException("end date for timepoint interval is before start date", startDate, endDate);
			}
		}
	TimepointInterval(const date::year_month& startDate, const date::year_month& endDate)
		: TimepointInterval(DataTypeConverter::monthYearToTimepoint(startDate), DataTypeConverter::monthYearToTimepoint(endDate))
	{
	}

	TimepointInterval(Timepoint date)
		: mStartDate(date), mEndDate(date) {}

	inline Timepoint getStartDate() const { return mStartDate; }
	inline Timepoint getEndDate() const { return mEndDate; }
	inline void setStartDate(Timepoint startDate) { mStartDate = startDate; }
	inline void setEndDate(Timepoint endDate) { mEndDate = endDate; }

	bool isEmpty() const { return mStartDate.time_since_epoch().count() == 0 && mEndDate.time_since_epoch().count() == 0; }
	bool isInsideInterval(date::month month, date::year year) const
	{
		auto startDate = date::year_month_day{ date::floor<date::days>(mStartDate) };
		if (year == startDate.year() && month < startDate.month()) { return false; }

		auto endDate = date::year_month_day{ date::floor<date::days>(mEndDate) };
		if (year == endDate.year() && month > endDate.month()) { return false; }

		if (year < startDate.year() || year > endDate.year()) { return false; }
		return true;
	}

	bool isInsideInterval(Timepoint date) const {
		return mStartDate <= date && date <= mEndDate;
	}

	// source: https://internalpointers.com/post/writing-custom-iterators-modern-cpp
	struct MonthYearIterator
	{
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t; // a month is a iterator step
		using value_type = date::year_month;
		using pointer = const value_type*;  // or also value_type*
		using reference = const value_type&;  // or also value_type&

		MonthYearIterator() = default;
		explicit MonthYearIterator(const value_type& v) : m_value(v) {};
		~MonthYearIterator() = default;		

		reference operator*() const { return m_value; }
		pointer operator->() { return &m_value; }

		// Prefix increment
		MonthYearIterator& operator++() { m_value += date::months(1); return *this; }

		// Prefix decrement
		MonthYearIterator& operator--() { m_value -= date::months(1); return *this; }

		// Postfix increment
		MonthYearIterator operator++(int) { MonthYearIterator tmp = *this; ++(*this); return tmp; }

		// Postfix decrement
		MonthYearIterator operator--(int) { MonthYearIterator tmp = *this; --(*this); return tmp; }

		friend bool operator== (const MonthYearIterator& a, const MonthYearIterator& b) {  return a.m_value == b.m_value; };
		friend bool operator!= (const MonthYearIterator& a, const MonthYearIterator& b) {  return a.m_value != b.m_value; };

	protected:

	private:
		value_type m_value;
	};

	MonthYearIterator begin() const {
		auto date = date::year_month_day{ date::floor<date::days>(mStartDate) };
		return MonthYearIterator({ date.year(), date.month() });
	}
	MonthYearIterator end() const {
		auto date = date::year_month_day{ date::floor<date::days>(mEndDate) };
		// +1 month to have end out of bounds
		date += date::months(1);
		return MonthYearIterator({ date.year(), date.month() });
	}
protected:
	Timepoint mStartDate;
	Timepoint mEndDate;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H