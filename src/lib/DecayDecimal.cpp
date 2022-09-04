#include "gradido_blockchain/lib/DecayDecimal.h"

DecayDecimal::~DecayDecimal()
{

}


void DecayDecimal::applyDecay(Poco::Timespan duration)
{
	Decimal durationSeconds(duration.totalSeconds());
	Decimal secondsPerYear(MAGIC_NUMBER_GREGORIAN_CALENDER_SECONDS_PER_YEAR);
	Decimal timeFactor = (durationSeconds / secondsPerYear);
	timeFactor = 2 ^ timeFactor;
	(*this) /= timeFactor;
}