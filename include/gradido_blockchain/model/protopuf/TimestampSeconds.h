#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TIMESTAMP_SECONDS_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TIMESTAMP_SECONDS_H

#include <bit>
#include "protopuf/message.h"

#include  <cstdint>
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/export.h"

using namespace pp;

namespace model {
	namespace protopuf {
		using TimestampSecondsMessage = message<
			int64_field<"seconds", 1>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT TimestampSeconds
		{
		public:
			TimestampSeconds(const TimestampSecondsMessage& data);
			TimestampSeconds(const Timepoint& date);

			~TimestampSeconds() {}

			Timepoint getAsTimepoint() const;

		protected:
			int64_t mSeconds;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TIMESTAMP_SECONDS_H