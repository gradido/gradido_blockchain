#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <cassert>

static const Timepoint DECAY_START_TIME = DataTypeConverter::dateTimeStringToTimePoint("2021-05-13 17:46:31");
constexpr double SECONDS_PER_YEAR = 31556952.0; // seconds in a year in gregorian calender

GradidoUnit::GradidoUnit(double gdd)
	: mGradidoCent(static_cast<int64_t>(roundToPrecision(gdd, 4) * 10000))
{

}

GradidoUnit::GradidoUnit(const std::string& stringAmount)
	: GradidoUnit(std::stod(stringAmount))
{
	
}

std::string GradidoUnit::toString() const
{
	std::stringstream ss;

	ss << std::fixed << std::setprecision(4);
	ss << static_cast<double>(*this);

	return ss.str();
}

double GradidoUnit::roundToPrecision(double GradidoUnit, uint8_t precision)
{
	auto factor = std::pow(10, precision);
	return std::round(GradidoUnit * factor) / factor;
}

int64_t GradidoUnit::calculateDecay(int64_t gradidoCent, int64_t seconds)
{
	if (seconds == 0) return gradidoCent;
	assert(seconds > 0);

	// decay for one year is 50%
	/*
	* while (seconds >= SECONDS_PER_YEAR) {
		gradidoCent *= 0.5;
		seconds -= SECONDS_PER_YEAR;
	}
	*/
	// optimize version from above
	if (seconds >= SECONDS_PER_YEAR) {
		auto times = static_cast<uint64_t>(seconds / SECONDS_PER_YEAR);
		seconds = seconds - times * SECONDS_PER_YEAR;
		gradidoCent = gradidoCent >> times;
		if (!seconds) return gradidoCent;
	}
//	*/
	/*!
	 *  calculate decay factor with compound interest formula converted to q <br>
	 *  n = (lg Kn - lg K0) / lg q => <br>
	 *  lg q = (lg Kn - lg K0) / n => <br>
	 *  q = e^((lg Kn - lg K0) / n)   <br>
	 * <br>
	 * with:
	 * <ul>
	 *  <li>q = decay_factor</li>
	 *  <li>n = days_per_year * 60 * 60 * 24 = seconds per year</li>
	 *  <li>Kn = 50 (capital after a year)</li>
	 *  <li>K0 = 100 (capital at start)</li>
	 * </ul>
	 * further simplified:
	 * lg 50 - lg 100 = lg 2 => 
	 * q = e^(lg 2 / n) = 2^(x/n)
	 * with x as seconds in which decay occured
	 */
	// https://www.wolframalpha.com/input?i=%28e%5E%28lg%282%29+%2F+31556952%29%29%5Ex&assumption=%7B%22FunClash%22%2C+%22lg%22%7D+-%3E+%7B%22Log%22%7D
	// from wolframalpha, based on the interest rate formula	
	auto decayFactor = pow(2, static_cast<double>(-seconds / SECONDS_PER_YEAR));
	
	return gradidoCent * decayFactor;
}

int64_t GradidoUnit::calculateCompoundInterest(int64_t gradidoCent, int64_t seconds)
{
	auto f = pow(1.5, seconds / SECONDS_PER_YEAR);
	return static_cast<int64_t>(static_cast<double>(gradidoCent) * pow(1.5, seconds / SECONDS_PER_YEAR));
}



Duration GradidoUnit::calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime)
{
	if(startTime > endTime) {
		throw EndDateBeforeStartDateException("startTime is before endTime in decay duration", startTime, endTime);
	}
	Timepoint start = startTime > DECAY_START_TIME ? startTime : DECAY_START_TIME;
	Timepoint end = endTime > DECAY_START_TIME ? endTime : DECAY_START_TIME;
	if (start == end) return std::chrono::seconds{ 0 };
	return end - start;
}
