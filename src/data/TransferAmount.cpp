#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		TransferAmount::TransferAmount(
			memory::ConstBlockPtr pubkeyPtr,
			const GradidoUnit& amount,
			const std::string& communityId /* = ""*/
		) : mPublicKey(pubkeyPtr), mAmount(amount), mCommunityId(communityId)
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
		}
	}
}