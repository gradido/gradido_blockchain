/*!
*
* \author: einhornimmond
*
* \date: 08.03.19
*
* \brief: easy to use time profiler
*/

#ifndef GRADIDO_BLOCKCHAIN_LIB_MONOTONIC_TIMER_H
#define GRADIDO_BLOCKCHAIN_LIB_MONOTONIC_TIMER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain_core/utils/mono_timer.h"

#include <string>

class GRADIDOBLOCKCHAIN_EXPORT MonotonicTimer
{
public:
	MonotonicTimer() {reset();};
	MonotonicTimer(const MonotonicTimer& copy) { mStartTick = copy.mStartTick; };
	~MonotonicTimer() {};

	void reset() { grdu_mono_timer_reset(&mStartTick); };
	double millis() const { return grdu_mono_timer_millis(mStartTick); };
	double micros() const { return grdu_mono_timer_micros(mStartTick); };
	double nanos() const { return grdu_mono_timer_nanos(mStartTick); };
	double seconds() const { return grdu_mono_timer_seconds(mStartTick); };
	std::string string() const;

protected:
	grdu_mono_timer mStartTick;
};

#endif //GRADIDO_BLOCKCHAIN_LIB_MONOTONIC_TIMER_H
