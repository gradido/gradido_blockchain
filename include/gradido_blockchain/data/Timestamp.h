#ifndef __GRADIDO_BLOCKCHAIN_DATA_TIMESTAMP_H
#define __GRADIDO_BLOCKCHAIN_DATA_TIMESTAMP_H

#include "TimestampSeconds.h"
#include "DurationSeconds.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT Timestamp
		{
		public:
			Timestamp() : mSeconds(0), mNanos(0) {}
			Timestamp(const Timepoint& date);
			Timestamp(int64_t _seconds, int32_t _nanos) : mSeconds(_seconds), mNanos(_nanos) {}

			inline operator Timepoint() const { return getAsTimepoint(); }
			Timepoint getAsTimepoint() const;
			inline int64_t getSeconds() const { return mSeconds; }
			inline int32_t getNanos() const { return mNanos; }
			std::string toString() const;

			inline bool operator==(const Timestamp& other) const { return mSeconds == other.mSeconds && mNanos == other.mNanos; }
			inline bool operator<(const Timestamp& other) const { return mSeconds < other.mSeconds || (mSeconds == other.mSeconds && mNanos < other.mNanos); }
			inline bool operator>(const Timestamp& other) const { return mSeconds > other.mSeconds || (mSeconds == other.mSeconds && mNanos > other.mNanos); }
			inline bool operator<(const TimestampSeconds& other) const { return mSeconds < other.getSeconds(); }
			inline bool operator>(const TimestampSeconds& other) const { return mSeconds > other.getSeconds(); }
			inline bool empty() const { return !mSeconds && !mNanos; }

			inline Timestamp operator+=(const DurationSeconds& duration) { mSeconds += static_cast<int64_t>(duration.getSeconds()); return *this; }

		protected:
			int64_t mSeconds;
			int32_t mNanos;
		};

		inline Timestamp operator+(const Timestamp& timestamp, const DurationSeconds& duration) {
			return Timestamp(timestamp.getSeconds() + duration.getSeconds(), timestamp.getNanos());
		}
	}
}



#endif //__GRADIDO_BLOCKCHAIN_DATA_TIMESTAMP_H