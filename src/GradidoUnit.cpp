#include "gradido_blockchain/GradidoUnit.h"

#include "loguru/loguru.hpp"

#include <chrono>
#include <string>

using std::string, std::stringstream, std::fixed, std::setprecision, std::pow, std::round;
using std::chrono::duration_cast, std::chrono::seconds;

GradidoUnit GradidoUnit::fromString(const string& stringAmount)
{
  grdd_unit gdd;
  if (!grdd_unit_from_string(&gdd, stringAmount.c_str())) {
    throw FixedPointedArithmetikOverflowException("rounding error with string", gdd);
  }
  return gdd;
}

string GradidoUnit::toString(int precision/* = 4*/) const
{
  string result(24, 0);
  auto resultSize = grdd_unit_to_string(result.data(), 24, mGradidoCent, precision);
  if (resultSize == INT_MAX) {
    throw GradidoNodeInvalidDataException("grdd_unit_to_string return to big string size value");
  }
  if (resultSize >= 24) {
    LOG_F(WARNING, "stack string buffer is to small for gradido unit string, size: %d, needed: %d for: %lu", 24, resultSize, mGradidoCent);
    auto resultSize2 = grdd_unit_to_string(result.data(), result.size(), mGradidoCent, precision);
    if (resultSize != resultSize2) {
      throw GradidoNodeInvalidDataException("grdd_unit_to_string work not like expected, it return different string size with same input");
    }
  }
  return result.substr(0, resultSize);
}

GradidoUnit GradidoUnit::roundToPrecision(uint8_t precision/* = 4*/) const {
	GradidoUnit result;
	if (!grdd_unit_round_to_precision(&result.mGradidoCent, mGradidoCent, precision)) {
		throw FixedPointedArithmetikOverflowException("rounding error with", mGradidoCent);
	}
	return result;
}

Duration GradidoUnit::calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime)
{
	auto startTimeSeconds = duration_cast<seconds>(startTime.time_since_epoch()).count();
	auto endTimeSeconds = duration_cast<seconds>(endTime.time_since_epoch()).count();

	grdd_duration_seconds duration;
	if(!grdd_unit_calculate_duration_seconds(startTimeSeconds, endTimeSeconds, &duration)) {
		throw EndDateBeforeStartDateException("startTime is before endTime in decay duration", startTime, endTime);
	}
	return seconds(duration);
}