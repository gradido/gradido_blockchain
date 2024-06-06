#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_COMMUNITY_ROOT_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/data/Protopuf.h"
#include "AbstractRole.h"
#include "BasicTypesRole.h"

#include <bit>
#include "protopuf/message.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				// gradido_transfer.proto
				using GradidoTransferMessage = message<
					message_field<"sender", 1, TransferAmountMessage>,
					bytes_field<"recipient", 2>//, pp::singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
				>;

				using GradidoDeferredTransferMessage = message<
					message_field<"transfer", 1, GradidoTransferMessage>,
					message_field<"timeout", 2, TimestampSecondsMessage>
				>;

				class GradidoTransferRole : public AbstractRole
				{
				public:
					GradidoTransferRole(const data::GradidoTransfer& data) : mData(data) {}
					~GradidoTransferRole() {};

					RUM_IMPLEMENTATION
					GradidoTransferMessage getMessage() const;
					virtual size_t calculateSerializedSize() const;

				protected:
					const data::GradidoTransfer& mData;
				};
			}
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_COMMUNITY_ROOT_ROLE_H