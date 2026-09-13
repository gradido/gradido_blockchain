#include "gradido_blockchain/lib/MonotonicTimer.h"
#include "arnm/mono_timer.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "loguru/loguru.hpp"

std::string MonotonicTimer::string() const
{
	char buffer[32];
	auto resultSize = arnm_mono_timer_string(buffer, 32, mStartTick);

	if (resultSize >= 32) {
		LOG_F(ERROR, "stack string buffer is to small for monotonic timer string, size: %d, needed: %d for: %lu", 32, resultSize, mStartTick);
		return "to big time value for string buffer";
	}
	return std::string(buffer);
}