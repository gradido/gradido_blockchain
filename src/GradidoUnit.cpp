#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <cassert>

static const Timepoint DECAY_START_TIME = DataTypeConverter::dateTimeStringToTimePoint("2021-05-13 17:46:31");

// Fixed-Point-Arithmetic Code by ChatGPT 4.0
// Fixed-point precision scaling factor
constexpr unsigned long long SCALE = 1LL << 32;
constexpr unsigned long long SECONDS_PER_YEAR = 31556952; // seconds in a year in gregorian calender
constexpr double LN2 = 0.6931471805599453; // ln(2)
//constexpr double LOG2 = 0.30102999566398119521373889472449;
constexpr unsigned long long MAX_GRADIDO_CENT_VALUE = std::numeric_limits<unsigned long long>::max() / SCALE;

// Function to calculate 2^x using fixed-point arithmetic
// should be more deterministic than using default pow function
unsigned long long fixed_point_exp2(unsigned long long x)
{
	/*double xd = static_cast<double>(x) / SCALE;
	double resultd = std::exp(xd * LOG2);
	return static_cast<int64_t>(resultd * SCALE);
	*/
	unsigned long long result = SCALE;
	unsigned long long term = SCALE;
	for (int i = 1; i < 50; ++i) {
		term = (term * x) / (SCALE * i);
		result += term;
		// Check for potential overflow
		if (term == 0) {
			break;
		};
	}
	return result;
	
}

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

uint64_t GradidoUnit::calculateDecayFactor(uint64_t seconds)
{
	if (seconds == 0) return SCALE;
	assert(seconds > 0);	

	// Convert duration to a fixed-point time factor
	unsigned long long timeFactor = (seconds * SCALE) / SECONDS_PER_YEAR;
	timeFactor = (timeFactor * LN2 * SCALE) / SCALE; // Multiply by LN2(2)

	// Compute 2^timeFactor in fixed-point
	return fixed_point_exp2(timeFactor);
}

int64_t GradidoUnit::calculateDecay(int64_t gradidoCent, Duration duration)
{
	uint64_t seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
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
		auto times = seconds / SECONDS_PER_YEAR;
		seconds = seconds - times * SECONDS_PER_YEAR;
		gradidoCent = gradidoCent >> times;
		if (!seconds) return gradidoCent;
	}
	unsigned long long decayFactor = calculateDecayFactor(seconds);
	
	return calculateDecayFast(gradidoCent, decayFactor);
}

int64_t GradidoUnit::calculateDecayFast(int64_t gradidoCent, uint64_t decayFactor)
{
	// If decayFactor is zero, return the original value to prevent division by zero
	if (decayFactor == 0) {
		return gradidoCent;
	}
	// prevent overflow with partly calculation
	if (gradidoCent > MAX_GRADIDO_CENT_VALUE) {
		uint64_t maxDecaied = std::numeric_limits<unsigned long long>::max() / decayFactor;
		uint64_t maxCount = gradidoCent / MAX_GRADIDO_CENT_VALUE;
		//auto rest = (gradidoCent - (maxCount * MAX_GRADIDO_CENT_VALUE)) * SCALE / decayFactor;
		uint64_t rest = gradidoCent - (maxCount * MAX_GRADIDO_CENT_VALUE);
		uint64_t restScaled = (rest * SCALE) / decayFactor;
		uint64_t maxCountScaled = maxCount * maxDecaied;

		// Check for potential overflow in the addition
		if (maxCountScaled > std::numeric_limits<int64_t>::max() - restScaled) {
			throw FixedPointedArithmetikOverflowException("calculateDecayFast overflow, gradido cent is to much to handle", gradidoCent);
		}

		return restScaled + maxCountScaled;
	}
	else {
		// Perform division in fixed-point and rescale
		return (gradidoCent * SCALE) / decayFactor;
	}
}


Duration GradidoUnit::calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime)
{
	assert(endTime > startTime);
	Timepoint start = startTime > DECAY_START_TIME ? startTime : DECAY_START_TIME;
	Timepoint end = endTime > DECAY_START_TIME ? endTime : DECAY_START_TIME;
	if (start == end) return std::chrono::seconds{ 0 };
	return end - start;
}
