#ifndef __GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H
#define __GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H

#include <bit>
#include "protopuf/message.h"
#include "VectorCacheAllocator.h"
#include "gradido_blockchain/MemoryManager.h"

using namespace pp;

namespace model {
	namespace protopuf {
		using SignaturePairMessage = message<
			bytes_field<"pubkey", 1, pp::singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			bytes_field<"signature", 2, pp::singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
		>;
		
		class SignaturePair
		{
		public: 
			SignaturePair(const SignaturePairMessage& data);
			~SignaturePair();

			inline const MemoryBin* getPubkey() const { return mPubkey; }
			inline const MemoryBin* getSignature() const { return mSignature; }

		protected:
			MemoryBin* mPubkey;
			MemoryBin* mSignature;
		};

	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_SIGNATURE_PAIR_H