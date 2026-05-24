#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/memory/Block.h"

using memory::Block, memory::ConstBlockPtr;
using std::string;

namespace gradido {
	namespace data {
		AccountBalance::AccountBalance()
			: mPublicKey(nullptr), mCoinCommunityIdIndex(0)
		{
		}

		AccountBalance::AccountBalance(ConstBlockPtr publicKey, GradidoUnit balance, uint32_t coinCommunityIdIndex)
			: mPublicKey(publicKey), mBalance(balance), mCoinCommunityIdIndex(coinCommunityIdIndex)
		{
		}

		AccountBalance::AccountBalance(ConstBlockPtr publicKey, GradidoUnit balance, const string& communityId)
			: mPublicKey(publicKey), mBalance(balance), mCoinCommunityIdIndex(g_appContext->getOrAddCommunityIdIndex(communityId))
		{
		}

		AccountBalance::AccountBalance(ConstBlockPtr publicKey, GradidoUnit balance, const Uuid& communityUuid)
			: mPublicKey(publicKey), mBalance(balance), mCoinCommunityIdIndex(g_appContext->getOrAddCommunityIdIndex(communityUuid))
		{

		}

		AccountBalance::~AccountBalance()
		{
		}
	}
}