#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "PublicKeyIndex.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT AccountBalance {
    GradidoUnit balance;
    PublicKeyIndex publicKeyIndex;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_ACCOUNT_BALANCE_H