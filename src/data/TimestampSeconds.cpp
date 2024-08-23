#include "gradido_blockchain/data/TimestampSeconds.h"

namespace gradido {
	namespace data {
		TimestampSeconds::TimestampSeconds(const Timepoint& date)
			: mSeconds(0)
		{
			// Get the duration since epoch
			auto duration = date.time_since_epoch();

			// Convert the duration to seconds
			mSeconds = duration_cast<std::chrono::seconds>(duration).count();
		}

		Timepoint TimestampSeconds::getAsTimepoint() const 
		{
			return std::chrono::system_clock::time_point(std::chrono::seconds{ mSeconds });
		}
	}
}