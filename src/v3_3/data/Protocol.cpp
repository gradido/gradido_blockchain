#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace data {

			Timestamp::Timestamp(const Timepoint& date)
				: seconds(0), nanos(0)
			{
				// Convert time_point to duration since epoch
				auto duration = date.time_since_epoch();

				// Convert duration to seconds and nanoseconds
				auto secondsDuration = duration_cast<std::chrono::seconds>(duration);
				seconds = secondsDuration.count();
				nanos = (duration_cast<std::chrono::nanoseconds>(duration) - duration_cast<std::chrono::nanoseconds>(secondsDuration)).count();
			}

			TimestampSeconds::TimestampSeconds(const Timepoint& date)
				: seconds(0)
			{
				// Get the duration since epoch
				auto duration = date.time_since_epoch();

				// Convert the duration to seconds
				seconds = duration_cast<std::chrono::seconds>(duration).count();
			}
		}
	}
}