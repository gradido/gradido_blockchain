#ifndef GRADIDO_BLOCKCHAIN_BLOCKCHAIN_DATA_RICH_ACCOUNTBALANCE_H
#define GRADIDO_BLOCKCHAIN_BLOCKCHAIN_DATA_RICH_ACCOUNTBALANCE_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/compact/AccountBalance.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/types.h"

namespace gradido::data {
  namespace rich {
    class GRADIDOBLOCKCHAIN_EXPORT AccountBalance 
    {
    public:
      AccountBalance();
      explicit AccountBalance(const compact::AccountBalance& data, uint32_t communityIdIndex, Timestamp balanceTime);
      ~AccountBalance() = default;

      inline Timestamp getBalanceTime() const { return mBalanceTime; }
      inline GradidoUnit getAmount() const { return mAmount; }
      inline compact::PublicKeyIndex getPublicKeyIndex() const { return mPublicKeyIndex; }
      inline uint32_t getCoinCommunityIdIndex() const { return mCoinCommunityIdIndex; }
      inline GradidoUnit getDecayedAmount(Timestamp to) const { return mAmount.calculateDecay(mBalanceTime, to); }
      inline bool empty() const { return mAmount == GradidoUnit::zero() && mPublicKeyIndex.publicKeyIndex == 0; }

    protected:
      Timestamp mBalanceTime;
      GradidoUnit mAmount;
      compact::PublicKeyIndex mPublicKeyIndex;      
      uint32_t mCoinCommunityIdIndex;      
    };
  }
}

#endif // GRADIDO_BLOCKCHAIN_BLOCKCHAIN_DATA_RICH_ACCOUNTBALANCE_H