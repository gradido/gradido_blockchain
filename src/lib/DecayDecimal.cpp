#include "gradido_blockchain/lib/DecayDecimal.h"

DecayDecimal::~DecayDecimal()
{

}


void DecayDecimal::applyDecay(Duration duration)
{
	Decimal durationSeconds(static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count()));
	Decimal secondsPerYear(MAGIC_NUMBER_GREGORIAN_CALENDER_SECONDS_PER_YEAR);
	Decimal timeFactor = (durationSeconds / secondsPerYear);
	timeFactor = 2 ^ timeFactor;
	(*this) /= timeFactor;
}