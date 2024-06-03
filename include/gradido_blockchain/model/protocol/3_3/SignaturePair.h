#ifndef __GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H
#define __GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H

#include <bit>
#include "protopuf/message.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/export.h"

using namespace pp;
using namespace memory;

namespace model {
	namespace protopuf {
		using SignaturePairMessage = message<
			bytes_field<"pubkey", 1, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			bytes_field<"signature", 2, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
		>;
		
		class GRADIDOBLOCKCHAIN_EXPORT SignaturePair
		{
		public: 
			SignaturePair(const SignaturePairMessage& data);
			
			//!
			//! \param pubkey will be copied
			//! \param signature will be copied
			SignaturePair(ConstMemoryBlockPtr pubkey, ConstMemoryBlockPtr signature);
			~SignaturePair() {}

			inline ConstMemoryBlockPtr getPubkey() const { return mPubkey; }
			inline ConstMemoryBlockPtr getSignature() const { return mSignature; }

		protected:
			ConstMemoryBlockPtr mPubkey;
			ConstMemoryBlockPtr mSignature;
		};

	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H