#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "r128.h"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

static const Timepoint DECAY_START_TIME = DataTypeConverter::dateTimeStringToTimePoint("2021-05-13 17:46:31");
constexpr double SECONDS_PER_YEAR = 31556952.0; // seconds in a year in gregorian calender

using std::string, std::stringstream, std::fixed, std::setprecision, std::pow, std::round;

constexpr int64_t POW10[] = { 1, 10, 100, 1000, 10000 };

GradidoUnit GradidoUnit::fromString(const std::string& stringAmount)
{
  const char* p = stringAmount.c_str();

		bool negative = false;

		if (*p == '-') {
        negative = true;
        ++p;
		}
    // --- integer part ---
    char* end;
    int64_t integerPart = strtoll(p, &end, 10);
    if (end == p && *p != '.') {
				throw InvalidGradidoUnitStringException("invalid GradidoUnit string: no digits found", stringAmount);
		}

    int64_t fractionalPart = 0;
    int digits = 0;

    p = end;

    // --- fractional part ---
    if (*p == '.') {
				++p;

				// why not using strtoll here also?
				// because we need only the next 4 numbers, we will check the 5th later because of rounding and skip the rest
        // first 4 digits
        while (isdigit(*p) && digits < 4) {
            fractionalPart = fractionalPart * 10 + (*p - '0');
            ++p;
            ++digits;
        }

        // pad with zeros
        while (digits < 4) {
            fractionalPart *= 10;
            ++digits;
        }

        // --- rounding digit (5th) ---
        if (isdigit(*p)) {
            int roundDigit = *p - '0';

            if (roundDigit >= 5) {
                fractionalPart += 1;

                // handle carry (e.g. 0.99995 -> 1.0000)
                if (fractionalPart >= 10000) {
                    fractionalPart = 0;
                    integerPart += 1;
                }
            }

            // skip remaining digits
            while (isdigit(*p)) p++;
        }
    }

    if (*p != '\0') throw InvalidGradidoUnitStringException("invalid GradidoUnit string: trailing characters", stringAmount);
		// int64 max:  9,223,372,036,854,775,807
		// int64 min: -9,223,372,036,854,775,807
		// int64 max for integer part (without fractional part): 922,337,203,685,476
    if (integerPart > 922337203685476 || integerPart < -922337203685476) {
				throw InvalidGradidoUnitStringException("invalid GradidoUnit string: integer part out of bounds [-922337203685476, 922337203685476]", stringAmount);
		}
		int64_t result = 0;
		if (negative) {
			integerPart *= -1;
			fractionalPart *= -1;
		}
		result = integerPart * 10000 + fractionalPart;

    return GradidoUnit(result);
}

std::string GradidoUnit::toString(int precision/* = 4*/) const
{
	if (precision > 4) precision = 4;

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
		memmove(&buffer[paddingCount + cursor], &buffer[cursor], numberPlacesCount);
		memset(&buffer[cursor], '0', paddingCount);
		cursor += paddingCount;
	}
	cursor += numberPlacesCount;
	// make room for .
	memmove(&buffer[cursor - 3], &buffer[cursor - 4], 5);
	cursor++;
	buffer[cursor - 5] = '.';

	if (precision != 4) {
		cursor -= 4 - precision;
		buffer[cursor] = '\0';
	}

	return string(buffer, cursor);
}

double GradidoUnit::roundToPrecisionDouble(double gradidoUnit, uint8_t precision)
{
	if (precision > 4) {
		precision = 4;
	}

	double factor = POW10[precision];
	return round(gradidoUnit * factor) / factor;
}

GradidoUnit GradidoUnit::roundToPrecision(uint8_t precision) const
{
	if (precision >= 4) return GradidoUnit(mGradidoCent);

	int shift = 4 - precision;
	uint64_t divisor = POW10[shift];

	// half-up rounding
	uint64_t half = divisor / 2;
	uint64_t rounded = 0;
	uint64_t gdd = mGradidoCent;
	if (mGradidoCent < 0) {
			gdd = -mGradidoCent;
	}
	rounded = ((gdd + half) / divisor) * divisor;
	if (rounded > 9223372036854775807) {
		throw FixedPointedArithmetikOverflowException("rounded value exceed int64 range", mGradidoCent);
	}
	if (mGradidoCent < 0) {
		return GradidoUnit::fromGradidoCent(-(int64_t)rounded);
	}
	return GradidoUnit::fromGradidoCent(rounded);
}

GradidoUnit GradidoUnit::calculateDecay(int64_t seconds) const
{
	if (seconds == 0) return mGradidoCent;

	// decay for one year is 50%
	/*
	* while (seconds >= SECONDS_PER_YEAR) {
		mGradidoCent *= 0.5;
		seconds -= SECONDS_PER_YEAR;
	}
	*/
	int64_t gradidoCent = mGradidoCent;
	// optimize version from above
	if (seconds >= SECONDS_PER_YEAR) {
		auto times = static_cast<uint64_t>(seconds / SECONDS_PER_YEAR);
		seconds = seconds - times * SECONDS_PER_YEAR;
		gradidoCent = mGradidoCent >> times;
		if (!seconds) return gradidoCent;
	}

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
	R128 factor = pow(2.0, static_cast<double>(-seconds) / SECONDS_PER_YEAR);
	auto decayed = R128(gradidoCent) * factor;
	R128 decayedAndRounded;
	r128Round(&decayedAndRounded, &decayed);
	return GradidoUnit::fromGradidoCent(r128ToInt(&decayedAndRounded));
	// return GradidoUnit(static_cast<int64_t>(static_cast<double>(gradidoCent) * pow(2.0, static_cast<double>(static_cast<double>(-seconds) / SECONDS_PER_YEAR))));
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