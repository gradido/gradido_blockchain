#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_TRANSFER_H

#include <bit>
#include "protopuf/message.h"

#include "gradido_blockchain/export.h"

#include "TransferAmount.h"

using namespace pp;

namespace model {
	namespace protopuf {

		using GradidoTransferMessage = message<
			message_field<"sender", 1, TransferAmountMessage>,
			bytes_field<"recipient", 2, pp::singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransfer
		{
		public:
			GradidoTransfer(const GradidoTransferMessage& data);			
			GradidoTransfer(const TransferAmount& sender, ConstCachedMemoryBlockPtr recipient);

			~GradidoTransfer() {}

			const TransferAmount& getSender() const { return mSender; }
			ConstCachedMemoryBlockPtr getRecipient() const { return mRecipient; }
		protected:
			TransferAmount mSender;
			ConstCachedMemoryBlockPtr mRecipient;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_TRANSFER_H