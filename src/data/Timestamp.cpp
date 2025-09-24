#include "gradido_blockchain/data/Timestamp.h"


namespace gradido {
	namespace data {

		Timestamp::Timestamp(const Timepoint& date)
			: mSeconds(0), mNanos(0)
		{
			// Convert time_point to duration since epoch
			auto duration = date.time_since_epoch();

			// Convert duration to seconds and nanoseconds
			auto secondsDuration = duration_cast<std::chrono::seconds>(duration);
			mSeconds = secondsDuration.count();
			mNanos = (duration_cast<std::chrono::nanoseconds>(duration) - duration_cast<std::chrono::nanoseconds>(secondsDuration)).count();
		}

		Timepoint Timestamp::getAsTimepoint() const 
		{
			int64_t microseconds = mSeconds * static_cast<int64_t>(1e6) + mNanos / static_cast<int64_t>(1e3);
			return std::chrono::system_clock::time_point(std::chrono::microseconds(microseconds));
		}
		std::string Timestamp::toString() const
		{
			std::string result;
			result.reserve(21);
			result = std::to_string(mSeconds);
			result.push_back('.');
			auto nanoString = std::to_string(mNanos);
			int zeroLeftPaddingCount = 9 - nanoString.size();
			while (zeroLeftPaddingCount--) {
				result.push_back('0');
			}
			result += nanoString;
			return result;
		}
	}
}