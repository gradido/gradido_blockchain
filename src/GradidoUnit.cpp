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
constexpr double LOG2 = 0.6931471805599453; // log(2)

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
	for (int i = 1; i < 20; ++i) {
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
		seconds = seconds % SECONDS_PER_YEAR;
		gradidoCent = gradidoCent >> times;
	}

	// Convert duration to a fixed-point time factor
	unsigned long long timeFactor = (seconds * SCALE) / SECONDS_PER_YEAR;
	timeFactor = (timeFactor * LOG2 * SCALE) / SCALE; // Multiply by log(2)

	// Compute 2^timeFactor in fixed-point
	unsigned long long decayFactor = fixed_point_exp2(timeFactor);
	
	// Perform division in fixed-point and rescale
	// return (gradidoCent * SCALE) / decayFactor;
	return (gradidoCent * SCALE) / decayFactor;
}

Duration GradidoUnit::calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime)
{
	assert(endTime > startTime);
	Timepoint start = startTime > DECAY_START_TIME ? startTime : DECAY_START_TIME;
	Timepoint end = endTime > DECAY_START_TIME ? endTime : DECAY_START_TIME;
	if (start == end) return std::chrono::seconds{ 0 };
	return end - start;
}
