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

	DecayDecimal(const Decimal& decimal);

	virtual ~DecayDecimal();
	
	virtual DecayDecimal calculateDecay(Duration duration) const;
	inline DecayDecimal calculateDecay(Timepoint startTime, Timepoint endTime) const {
		return calculateDecay(calculateDecayDurationSeconds(startTime, endTime));
	}

#ifdef USE_MPFR
	inline bool operator>(int b) { return mpfr_cmp_si(mDecimal, b) > 0; };	
	inline bool operator<(int b) { return mpfr_cmp_si(mDecimal, b) < 0; };
	inline bool operator<(DecayDecimal& b) { return mpfr_cmp(mDecimal, b.mDecimal) < 0; };
#else
	inline bool operator<(const DecayDecimal& b) const { return mDecimal < b; };
	inline bool operator>(const DecayDecimal& b) const { return mDecimal > b; };
#endif
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DECAY_DECIMAL_H