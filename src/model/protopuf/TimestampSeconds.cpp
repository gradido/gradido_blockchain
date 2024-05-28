#include "gradido_blockchain/model/protopuf/TimestampSeconds.h"

using namespace std::chrono;

namespace model {
	namespace protopuf {
		TimestampSeconds::TimestampSeconds(const TimestampSecondsMessage& data)
		{
			mSeconds = data["seconds"_f].value();
		}

		Timepoint TimestampSeconds::getAsTimepoint() const
		{
			return system_clock::time_point( std::chrono::seconds{ mSeconds} );
		}
	}
}

