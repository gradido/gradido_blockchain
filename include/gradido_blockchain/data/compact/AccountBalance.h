#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H

#include "gradido_blockchain/types.h"

namespace gradido::data::compact {
  // account balance belong to community
  struct AccountBalance 
  {
    int64_t     balanceGddCent;
    uint32_t    coinCommunityIdIndex;
    uint32_t    publicKeyIndex;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H