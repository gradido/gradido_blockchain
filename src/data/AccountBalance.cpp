#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain_core/data/wire/basic_types.h"

#include <memory>

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;
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

		AccountBalance::AccountBalance(const grdw_account_balance& coreAccountBalance)
			: mPublicKey(make_shared<const Block>(coreAccountBalance.pubkey)), 
			mBalance(GradidoUnit::fromGradidoCent(coreAccountBalance.balance)),
			mCoinCommunityIdIndex(g_appContext->getOrAddCommunityIdIndex(coreAccountBalance.community_uuid))
		{

		}

		AccountBalance::~AccountBalance()
		{
		}
	}
}