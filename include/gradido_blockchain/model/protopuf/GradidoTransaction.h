#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_TRANSACTION_H

#include "SignatureMap.h"

namespace model {
	namespace protopuf {
		using GradidoTransactionMessage = message<
			message_field<"sig_map", 1, SignatureMapMessage>,
			bytes_field<"body_bytes", 2, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			bytes_field<"parent_message_id", 3, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction
		{
		public:
			GradidoTransaction(const GradidoTransactionMessage& data);
			GradidoTransaction(
				const SignaturePair& firstSignaturePair,
				ConstCachedMemoryBlockPtr bodyBytes,
				ConstCachedMemoryBlockPtr parentMessageId = nullptr
			);

			~GradidoTransaction() {}

			inline const SignatureMap& getSignatureMap() { return mSignatureMap; }
			inline ConstCachedMemoryBlockPtr getBodyBytes() { return mBodyBytes; }
		protected:
			SignatureMap			mSignatureMap;
			ConstCachedMemoryBlockPtr	mBodyBytes;
			ConstCachedMemoryBlockPtr	mParentMessageId;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_TRANSACTION_H