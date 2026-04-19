#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#define R128_IMPLEMENTATION
#include "r128.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <cassert>

static const Timepoint DECAY_START_TIME = DataTypeConverter::dateTimeStringToTimePoint("2021-05-13 17:46:31");
constexpr long double SECONDS_PER_YEAR = 31556952.0; // seconds in a year in gregorian calender

using std::string, std::stringstream, std::fixed, std::setprecision, std::pow, std::round;

constexpr int64_t POW10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };

GradidoUnit GradidoUnit::fromString(const std::string& stringAmount)
{
		R128 gradidoPrecise;
		r128FromString(&gradidoPrecise, stringAmount.c_str(), nullptr);
		return gradidoPrecise;
		
}

std::string GradidoUnit::toString(int precision/* = 4*/) const
{
	auto rounded = roundToPrecision(precision).mGradidoPrecise;
	R128ToStringFormat options{
		.sign = R128ToStringSign_Default,
		.width = 1 + precision,
		.precision = precision,
		.zeroPad = 0,
		.decimal = 0,
		.leftAlign = 0 // 0 = means right align
	};
	if (!rounded.lo) {
		options.width = 0;
	}
	char buffer[42];
	auto written = r128ToStringOpt(buffer, 42, &rounded, &options);
	return string(buffer, written);
	/*if (precision > 4) precision = 4;

	auto rounded = roundToPrecision(precision).getGradidoCent();

	bool negative = rounded < 0;

	const size_t bufferSize = 32; // enough for int64 with 4 decimal places and null terminator
	char buffer[bufferSize];
	size_t cursor = 0;

	if (negative) {
		rounded *= -1;
		buffer[cursor++] = '-';
	}
	if (!precision) {
		int64_t integerPart = rounded / 10000;
		cursor += DataTypeConverter::uint64ToString(integerPart, &buffer[cursor]);
		return string(buffer, cursor);
	}
	// int64_t integerPart = rounded / 10000;
	// int64_t fractional = rounded - integerPart * 10000;
	
	auto numberPlacesCount = DataTypeConverter::uint64ToString(rounded, &buffer[cursor]);
	// pad with 0
	if (numberPlacesCount < 5) {
		auto paddingCount = 5 - numberPlacesCount;
		memcpy(&buffer[paddingCount + cursor], &buffer[cursor], numberPlacesCount);
		memset(&buffer[cursor], '0', paddingCount);
		cursor += paddingCount;
	}
	cursor += numberPlacesCount;
	// make room for .
	memcpy(&buffer[cursor - 3], &buffer[cursor - 4], 5);
	cursor++;
	buffer[cursor - 5] = '.';
	
	if (precision != 4) {
		cursor -= 4 - precision;
		buffer[cursor] = '\0';
	}
	
	return string(buffer, cursor);
	*/
}

double GradidoUnit::roundToPrecisionDouble(R128 gradidoUnit, uint8_t precision)
{
	if (precision > 4) {
		precision = 4;
	}

	R128 factor = POW10[precision];
	R128 scaled = gradidoUnit * factor;
	R128 scaledRounded;
	r128Round(&scaledRounded, &scaled);
	return scaledRounded / factor;
}

GradidoUnit GradidoUnit::roundToPrecision(uint8_t precision) const
{
	if (precision >= 8) precision = 8;
	
	R128 factor = POW10[precision];

	R128 scaled = mGradidoPrecise * factor;
	R128 rounded;
	r128Round(&rounded, &scaled);
	return rounded / factor;
}

GradidoUnit GradidoUnit::calculateDecay(int64_t seconds) const
{
	if (seconds == 0) return GradidoUnit(mGradidoPrecise);
	
	//	*/
	/*!
	 *  calculate decay factor with compound interest formula converted to q <br>
	 *  n = (lg Kn - lg K0) / lg q => <br>
	 *  lg q = (lg Kn - lg K0) / n => <br>
	 *  q = e^((lg Kn - lg K0) / n)   <br>
	 * <br>
	 * with:bufferSize
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
	// return GradidoUnit(static_cast<int64_t>(static_cast<double>(gradidoCent) * pow(2.0, static_cast<double>(static_cast<double>(-seconds) / SECONDS_PER_YEAR))));
	R128 factorDouble((double)pow(2.0, -static_cast<double>(seconds) / SECONDS_PER_YEAR));
	// 3. Multipliziere den Betrag mit dem skalierten Faktor
	return mGradidoPrecise * factorDouble;
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