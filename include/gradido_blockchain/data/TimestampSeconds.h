#ifndef __GRADIDO_BLOCKCHAIN_DATA_TIMESTAMP_SECONDS_H
#define __GRADIDO_BLOCKCHAIN_DATA_TIMESTAMP_SECONDS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT TimestampSeconds
		{
		public:
			TimestampSeconds() : mSeconds(0) {}
			TimestampSeconds(const Timepoint& date);
			TimestampSeconds(int64_t seconds) : mSeconds(seconds) {}

			operator Timepoint() const { return getAsTimepoint(); }
			Timepoint getAsTimepoint() const;
			inline int64_t getSeconds() const { return mSeconds; }

			bool operator==(const TimestampSeconds& other) const { return mSeconds == other.mSeconds; }
			bool operator!=(const TimestampSeconds& other) const { return mSeconds != other.mSeconds; }
			bool operator<(const TimestampSeconds& other) const { return mSeconds < other.mSeconds; }
			bool operator<=(const TimestampSeconds& other) const { return mSeconds <= other.mSeconds; }
			bool operator>(const TimestampSeconds& other) const { return mSeconds > other.mSeconds; }
			bool operator>=(const TimestampSeconds& other) const { return mSeconds >= other.mSeconds; }

		protected:
			int64_t mSeconds;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_TIMESTAMP_SECONDS_H