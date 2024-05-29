#ifndef __GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H
#define __GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H

#include <bit>
#include "protopuf/message.h"
#include "VectorCacheAllocator.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/export.h"

using namespace pp;

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
			SignaturePair(ConstCachedMemoryBlockPtr pubkey, ConstCachedMemoryBlockPtr signature);
			~SignaturePair() {}

			inline ConstCachedMemoryBlockPtr getPubkey() const { return mPubkey; }
			inline ConstCachedMemoryBlockPtr getSignature() const { return mSignature; }

		protected:
			ConstCachedMemoryBlockPtr mPubkey;
			ConstCachedMemoryBlockPtr mSignature;
		};

	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H