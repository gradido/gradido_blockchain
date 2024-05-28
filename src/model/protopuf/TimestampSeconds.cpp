#include "gradido_blockchain/model/protopuf/TimestampSeconds.h"

using namespace std::chrono;

namespace model {
	namespace protopuf {
		TimestampSeconds::TimestampSeconds(const TimestampSecondsMessage& data)
		{
			mSeconds = data["seconds"_f].value();
		}

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
			return system_clock::time_point( std::chrono::seconds{ mSeconds} );
		}
	}
}

