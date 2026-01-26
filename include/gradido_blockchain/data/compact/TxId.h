#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TX_ID_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TX_ID_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT TxId
  {

    TxId() : nr(0), communityIdIndex(0) {}
    TxId(uint64_t nr, uint32_t communityIdIndex) : nr(nr), communityIdIndex(communityIdIndex) {}

    // transaction nr, start with 1
    uint64_t nr;
    // index, start with 0
    uint32_t communityIdIndex;
    bool isEmpty() const { return nr == 0 && communityIdIndex == 0; }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TX_ID_H