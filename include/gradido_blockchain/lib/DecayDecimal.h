#ifndef __GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H
#define __GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H

#include "Decimal.h"
#include "Decay.h"

class DecayDecimal : public Decimal
{
public:
	// inherit constructors
	using Decimal::Decimal;
	virtual ~DecayDecimal();

	static Decimal calculateDecayFactor(Poco::Timespan duration);
	static inline Decimal calculateDecayFactor(Poco::Timestamp startTime, Poco::Timestamp endTime) {
		return calculateDecayFactor(calculateDecayDurationSeconds(startTime, endTime));
	}

	virtual void applyDecay(Poco::Timespan duration);
	inline void applyDecay(Poco::Timestamp startTime, Poco::Timestamp endTime) {
		applyDecay(calculateDecayDurationSeconds(startTime, endTime));
	}

};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H