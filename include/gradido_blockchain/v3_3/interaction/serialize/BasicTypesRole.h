#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_BASIC_TYPES_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_BASIC_TYPES_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/data/Protopuf.h"
#include "AbstractRole.h"

#include <bit>
#include "protopuf/message.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				using SignaturePairMessage = message<
					bytes_field<"pubkey", 1>,
					bytes_field<"signature", 2>
				>;

				using SignatureMapMessage = message<
					message_field<"sig_pair", 1, SignaturePairMessage, repeated>
				>;

				using TimestampMessage = message<
					int64_field<"seconds", 1>,
					int32_field<"nanos", 2>
				>;

				using TimestampSecondsMessage = message<
					int64_field<"seconds", 1>
				>;

				using TransferAmountMessage = message<
					bytes_field<"pubkey", 1>,
					string_field<"amount", 2>,
					string_field<"community_id", 3>
				>;

			}
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_BASIC_TYPES_ROLE_H