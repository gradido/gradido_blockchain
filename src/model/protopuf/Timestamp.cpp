#include "gradido_blockchain/model/protopuf/Timestamp.h"

using namespace std::chrono;

namespace model {
	namespace protopuf {
		Timestamp::Timestamp(const TimestampMessage& data) 
		{
			mSeconds = data["seconds"_f].value();
			mNanos = data["nanos"_f].value();
		}

		Timepoint Timestamp::getAsTimepoint() const
		{
			int64_t microseconds = mSeconds * static_cast<int64_t>(1e6) + mNanos / static_cast<int64_t>(1e3);
			return system_clock::time_point(std::chrono::microseconds(microseconds));
		}
	}
}

