#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		TransferAmount::TransferAmount(
			memory::ConstBlockPtr pubkeyPtr,
			const std::string& amountString,
			const std::string& communityId /* = ""*/
		) : mPubkey(pubkeyPtr), mAmount(amountString), mCommunityId(communityId)
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
		}
	}
}