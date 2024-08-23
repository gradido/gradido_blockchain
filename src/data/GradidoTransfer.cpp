#include "gradido_blockchain/data/GradidoTransfer.h"

namespace gradido {
	namespace data {
		bool GradidoTransfer::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			return publicKey->isTheSame(mSender.getPubkey()) || publicKey->isTheSame(mRecipient);
		}
	}
}