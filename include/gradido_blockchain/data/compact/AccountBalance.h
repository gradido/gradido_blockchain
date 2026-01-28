#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"

namespace gradido::data::compact {
  // account balance belong to community
  struct GRADIDOBLOCKCHAIN_EXPORT AccountBalance 
  {
    GradidoUnit balance;
    uint32_t    coinCommunityIdIndex;
    uint32_t    publicKeyIndex;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H