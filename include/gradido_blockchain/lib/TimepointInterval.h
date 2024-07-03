#ifndef __GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H
#define __GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H

#include "gradido_blockchain/types.h"
#include "date/date.h"

class TimepointInterval
{
public:
	TimepointInterval() {}

	TimepointInterval(Timepoint startDate, Timepoint endDate)
		: mStartDate(startDate), mEndDate(endDate) {}

	TimepointInterval(Timepoint date)
		: mStartDate(date), mEndDate(date) {}

	inline Timepoint getStartDate() const { return mStartDate; }
	inline Timepoint getEndDate() const { return mEndDate; }

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
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t; // a month is a iterator step
		using value_type = date::year_month;
		using pointer = value_type*;  // or also value_type*
		using reference = value_type&;  // or also value_type&

		MonthYearIterator(pointer ptr)
			: m_ptr(ptr) {}

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }

		// Prefix increment
		MonthYearIterator& operator++() { (*m_ptr) += date::months(1); return *this; }

		// Postfix increment
		MonthYearIterator operator++(int) { MonthYearIterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const MonthYearIterator& a, const MonthYearIterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const MonthYearIterator& a, const MonthYearIterator& b) { return a.m_ptr != b.m_ptr; };

	private:
		pointer m_ptr;
	};
	
protected:
	Timepoint mStartDate;
	Timepoint mEndDate;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H