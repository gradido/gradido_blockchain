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
#include "arnm/mono_timer.h"

#include <string>

class GRADIDOBLOCKCHAIN_EXPORT MonotonicTimer
{
public:
	MonotonicTimer() {reset();};
	MonotonicTimer(const MonotonicTimer& copy) { mStartTick = copy.mStartTick; };
	~MonotonicTimer() {};

	inline void reset() { arnm_mono_timer_reset(&mStartTick); };
	inline double millis() const { return arnm_mono_timer_millis(mStartTick); };
	inline double micros() const { return arnm_mono_timer_micros(mStartTick); };
	inline double nanos() const { return arnm_mono_timer_nanos(mStartTick); };
	inline double seconds() const { return arnm_mono_timer_seconds(mStartTick); };
	std::string string() const;
	inline size_t string(char* buffer, size_t bufferSize) const {
		return arnm_mono_timer_string(buffer, bufferSize, mStartTick);
	}

protected:
	arnm_mono_timer mStartTick;
};

#endif //GRADIDO_BLOCKCHAIN_LIB_MONOTONIC_TIMER_H
