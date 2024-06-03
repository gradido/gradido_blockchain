#include "gradido_blockchain/model/protopuf/GradidoTransfer.h"

namespace model {
	namespace protopuf {
		GradidoTransfer::GradidoTransfer(const GradidoTransferMessage& data)
			: mSender(data["sender"_f].value()), mRecipient(std::make_shared<memory::Block>(data["recipient"_f].value()))
		{
		}

		GradidoTransfer::GradidoTransfer(const TransferAmount& sender, ConstMemoryBlockPtr recipient)
			: mSender(sender), mRecipient(recipient)
		{
		}
	}
}