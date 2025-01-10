#ifndef __GRADIDO_BLOCKCHAIN_DATA_DURATION_SECONDS_H
#define __GRADIDO_BLOCKCHAIN_DATA_DURATION_SECONDS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "date/date.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT DurationSeconds
		{
		public:
			DurationSeconds() : mSeconds(0) {}
			DurationSeconds(const Duration& duration);
			DurationSeconds(uint32_t seconds) : mSeconds(seconds) {}

			operator Duration() const { return getAsDuration(); }
			Duration getAsDuration() const;
			inline uint32_t getSeconds() const { return mSeconds; }

			bool operator==(const DurationSeconds& other) const { return mSeconds == other.mSeconds; }
			bool operator!=(const DurationSeconds& other) const { return mSeconds != other.mSeconds; }
			bool operator<(const DurationSeconds& other) const { return mSeconds < other.mSeconds; }
			bool operator<=(const DurationSeconds& other) const { return mSeconds <= other.mSeconds; }
			bool operator>(const DurationSeconds& other) const { return mSeconds > other.mSeconds; }
			bool operator>=(const DurationSeconds& other) const { return mSeconds >= other.mSeconds; }

		protected:
			uint32_t mSeconds;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_DURATION_SECONDS_H