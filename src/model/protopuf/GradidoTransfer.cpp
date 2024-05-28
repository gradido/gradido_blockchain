#include "gradido_blockchain/model/protopuf/GradidoTransfer.h"

namespace model {
	namespace protopuf {
		GradidoTransfer::GradidoTransfer(const GradidoTransferMessage& data)
			: mSender(data["sender"_f].value()), mRecipient(std::make_shared<CachedMemoryBlock>(data["recipient"_f].value()))
		{
		}

		GradidoTransfer::GradidoTransfer(const TransferAmount& sender, ConstCachedMemoryBlockPtr recipient)
			: mSender(sender), mRecipient(recipient)
		{
		}
	}
}