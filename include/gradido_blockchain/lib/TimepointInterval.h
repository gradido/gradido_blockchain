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
		using pointer = value_type*;  // or also value_type*
		using reference = value_type&;  // or also value_type&

		MonthYearIterator(pointer ptr)
			: m_ptr(ptr) 
		{
		}

		// Copy constructor
		MonthYearIterator(const MonthYearIterator& other)
			: m_ptr(new value_type(other.m_ptr->year(), other.m_ptr->month())) // Deep copy of the value
		{
		}

		// Move constructor
		MonthYearIterator(MonthYearIterator&& other) noexcept
			: m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr; // Leave other in a valid state
		}

		~MonthYearIterator()
		{
			if (m_ptr) {
				delete m_ptr;
			}
		}

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }

		// Prefix increment
		MonthYearIterator& operator++() { (*m_ptr) += date::months(1); return *this; }

		// Prefix decrement
		MonthYearIterator& operator--() { (*m_ptr) -= date::months(1); return *this; }

		// Postfix increment
		MonthYearIterator operator++(int) { MonthYearIterator tmp = *this; ++(*this); return tmp; }

		// Postfix decrement 
		MonthYearIterator operator--(int) { MonthYearIterator tmp = *this; --(*this); return tmp; }

		friend bool operator== (const MonthYearIterator& a, const MonthYearIterator& b) { return *a.m_ptr == *b.m_ptr; };
		friend bool operator!= (const MonthYearIterator& a, const MonthYearIterator& b) { return *a.m_ptr != *b.m_ptr; };

	private:
		pointer m_ptr;
	};

	MonthYearIterator begin() { 
		auto date = date::year_month_day{ date::floor<date::days>(mStartDate) };
		return MonthYearIterator(new date::year_month(date.year(), date.month()));
	}
	MonthYearIterator end() { 
		auto date = date::year_month_day{ date::floor<date::days>(mEndDate) };
		// +1 month to have end out of bounds
		date += date::months(1);
		return MonthYearIterator(new date::year_month(date.year(), date.month()));
	}
	
protected:
	Timepoint mStartDate;
	Timepoint mEndDate;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_TIMEPOINT_INTERVAL_H