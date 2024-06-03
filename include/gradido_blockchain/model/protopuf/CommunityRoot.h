#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_ROOT_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_ROOT_H

#include <bit>
#include "protopuf/message.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/export.h"

using namespace pp;
using namespace memory;

namespace model {
	namespace protopuf {

		using CommunityRootMessage = message <
			bytes_field<"pubkey", 1, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			bytes_field<"gmw_pubkey", 2, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			bytes_field<"auf_pubkey", 3, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT CommunityRoot
		{
		public:
			CommunityRoot(const CommunityRootMessage& data);
			CommunityRoot(
				ConstMemoryBlockPtr pubkey,
				ConstMemoryBlockPtr gmwPubkey,
				ConstMemoryBlockPtr aufPubkey
			);
			~CommunityRoot() {}

			inline ConstMemoryBlockPtr getPubkey() const { return mPubkey; }
			inline ConstMemoryBlockPtr getGmwPubkey() const { return mGmwPubkey; }
			inline ConstMemoryBlockPtr getAufPubkey() const { return mAufPubkey; }

			/*operator CommunityRootMessage() const {
				return CommunityRootMessage{ mPubkey->bytes(), mGmwPubkey->bytes(), mAufPubkey->bytes() };
			}*/

		protected:
			ConstMemoryBlockPtr mPubkey;
			ConstMemoryBlockPtr mGmwPubkey;
			ConstMemoryBlockPtr mAufPubkey;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_ROOT_H