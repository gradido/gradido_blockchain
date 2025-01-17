#include "gradido_blockchain/data/DurationSeconds.h"

namespace gradido {
	namespace data {
		DurationSeconds::DurationSeconds(const Duration& duration)
			: mSeconds(0)
		{
			// Convert the duration to seconds
			mSeconds = duration_cast<std::chrono::seconds>(duration).count();
		}

		Duration DurationSeconds::getAsDuration() const
		{
			return std::chrono::seconds{ mSeconds };
		}
	}
}