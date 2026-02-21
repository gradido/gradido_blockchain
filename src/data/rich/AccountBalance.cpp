#include "gradido_blockchain/data/compact/AccountBalance.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/rich/AccountBalance.h"
#include "gradido_blockchain/data/Timestamp.h"

namespace gradido::data {
  namespace rich {
    AccountBalance::AccountBalance()
      : mPublicKeyIndex{}, mAmount(GradidoUnit::zero()), mCoinCommunityIdIndex(0)
    {

    }
    AccountBalance::AccountBalance(const compact::AccountBalance& data, uint32_t communityIdIndex, Timestamp balanceTime)
      : mBalanceTime(balanceTime),
      mAmount(GradidoUnit::fromGradidoCent(data.balanceGddCent)), 
      mPublicKeyIndex({ .communityIdIndex = communityIdIndex, .publicKeyIndex = data.publicKeyIndex }),
      mCoinCommunityIdIndex(data.coinCommunityIdIndex)
    {

    }
  }
}