#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TIMESTAMP_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TIMESTAMP_H

#include <bit>
#include "protopuf/message.h"

#include  <cstdint>
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/export.h"

using namespace pp;

namespace model {
	namespace protopuf {
		using TimestampMessage = message<
			int64_field<"seconds", 1>,
			int32_field<"nanos", 2>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT Timestamp
		{
		public: 
			Timestamp(const TimestampMessage& data);
			Timestamp(const Timepoint& date);
			~Timestamp() {}

			Timepoint getAsTimepoint() const;

			operator TimestampMessage() const {
				return TimestampMessage{ mSeconds, mNanos };
			}

		protected:
			int64_t mSeconds;
			int32_t mNanos;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TIMESTAMP_H