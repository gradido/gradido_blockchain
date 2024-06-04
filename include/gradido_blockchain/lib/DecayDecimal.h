#ifndef __GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H
#define __GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H

#include "Decimal.h"
#include "Decay.h"

#define MAGIC_NUMBER_GREGORIAN_CALENDER_SECONDS_PER_YEAR 31556952

class GRADIDOBLOCKCHAIN_EXPORT DecayDecimal : public Decimal
{
public:
	// inherit constructors
	using Decimal::Decimal;
	virtual ~DecayDecimal();
	
	virtual void applyDecay(Duration duration);
	inline void applyDecay(Timepoint startTime, Timepoint endTime) {
		applyDecay(calculateDecayDurationSeconds(startTime, endTime));
	}
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H