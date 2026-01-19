#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		TransferAmount::TransferAmount(
			memory::ConstBlockPtr pubkeyPtr,
			const GradidoUnit& amount,
			uint32_t coinCommunityIdIndex
		) : mPublicKey(pubkeyPtr), mAmount(amount), mCoinCommunityIdIndex(coinCommunityIdIndex)
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
		}
	}
}