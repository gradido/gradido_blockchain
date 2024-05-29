#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_ROOT_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_ROOT_H

#include <bit>
#include "protopuf/message.h"
#include "VectorCacheAllocator.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/export.h"

using namespace pp;

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
				ConstCachedMemoryBlockPtr pubkey,
				ConstCachedMemoryBlockPtr gmwPubkey,
				ConstCachedMemoryBlockPtr aufPubkey
			);
			~CommunityRoot() {}

			inline ConstCachedMemoryBlockPtr getPubkey() const { return mPubkey; }
			inline ConstCachedMemoryBlockPtr getGmwPubkey() const { return mGmwPubkey; }
			inline ConstCachedMemoryBlockPtr getAufPubkey() const { return mAufPubkey; }

			/*operator CommunityRootMessage() const {
				return CommunityRootMessage{ mPubkey->bytes(), mGmwPubkey->bytes(), mAufPubkey->bytes() };
			}*/

		protected:
			ConstCachedMemoryBlockPtr mPubkey;
			ConstCachedMemoryBlockPtr mGmwPubkey;
			ConstCachedMemoryBlockPtr mAufPubkey;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_ROOT_H