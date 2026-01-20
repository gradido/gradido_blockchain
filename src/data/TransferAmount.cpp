#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/memory/Block.h"

using memory::ConstBlockPtr;

namespace gradido {
	namespace data {
		TransferAmount::TransferAmount(
			ConstBlockPtr pubkeyPtr,
			const GradidoUnit& amount,
			uint32_t coinCommunityIdIndex
		) : mPublicKey(pubkeyPtr), mAmount(amount), mCoinCommunityIdIndex(coinCommunityIdIndex)
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
		}

		TransferAmount::TransferAmount(
			ConstBlockPtr pubkeyPtr,
			const GradidoUnit& amount,
			const std::string& coinCommunityId
		) : mPublicKey(pubkeyPtr), mAmount(amount)
		{
			mCoinCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(coinCommunityId);
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
		}
	}
}