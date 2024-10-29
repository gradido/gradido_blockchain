#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		GradidoTransfer::GradidoTransfer(const TransferAmount& _sender, memory::ConstBlockPtr recipientPtr)
			: mSender(_sender), mRecipient(recipientPtr) 
		{
			KeyPairEd25519::validatePublicKey(recipientPtr);
		}

		bool GradidoTransfer::isInvolved(const memory::Block& publicKey) const
		{
			return publicKey.isTheSame(mSender.getPubkey()) || publicKey.isTheSame(mRecipient);
		}
	}
}