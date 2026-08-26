#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include <string>

using memory::ConstBlockPtr;
using std::string, std::to_string;

namespace gradido {
	namespace data {
		TransferAmount::TransferAmount(
			ConstBlockPtr pubkeyPtr,
			const GradidoUnit& amount,
			uint32_t coinCommunityIdIndex
		) : mPublicKey(pubkeyPtr), mAmount(amount), mCoinCommunityIdIndex(coinCommunityIdIndex)
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
			if (!g_appContext->getCommunityIds().getDataForIndex(coinCommunityIdIndex)) {
				throw DictionaryMissingEntryException("invalid coin community id index in TransferAmount Construct", to_string(coinCommunityIdIndex));
			}
		}

		TransferAmount::TransferAmount(
			ConstBlockPtr pubkeyPtr,
			const GradidoUnit& amount,
			const string& coinCommunityId
		) : mPublicKey(pubkeyPtr), mAmount(amount), mCoinCommunityIdIndex(g_appContext->getOrAddCommunityIdIndex(coinCommunityId))
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
		}
	}
}