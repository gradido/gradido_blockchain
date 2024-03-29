#ifndef __GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H
#define __GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H

#include "Decimal.h"
#include "Decay.h"

#define MAGIC_NUMBER_GREGORIAN_CALENDER_SECONDS_PER_YEAR 31556952

class DecayDecimal : public Decimal
{
public:
	// inherit constructors
	using Decimal::Decimal;
	virtual ~DecayDecimal();
	
	virtual void applyDecay(Poco::Timespan duration);
	inline void applyDecay(Poco::Timestamp startTime, Poco::Timestamp endTime) {
		applyDecay(calculateDecayDurationSeconds(startTime, endTime));
	}
	inline void applyDecay(Poco::DateTime startTime, Poco::DateTime endTime) {
		applyDecay(calculateDecayDurationSeconds(startTime.timestamp(), endTime.timestamp()));
	}

};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H