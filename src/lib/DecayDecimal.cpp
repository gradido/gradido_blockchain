#include "gradido_blockchain/lib/DecayDecimal.h"

DecayDecimal::~DecayDecimal()
{

}

Decimal DecayDecimal::calculateDecayFactor(Poco::Timespan duration)
{
	auto durationSeconds = duration.totalSeconds();
	if (!durationSeconds) {
		return 1;
	}
	else {
		Decimal decayForDuration;
		mpfr_pow_ui(decayForDuration, gDecayFactorGregorianCalender, durationSeconds, gDefaultRound);
		return std::move(decayForDuration);
	}
}

void DecayDecimal::applyDecay(Poco::Timespan duration)
{
	auto decay = calculateDecayFactor(duration);
	(*this) *= decay;
}