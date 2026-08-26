#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__

#include "gradido_blockchain/export.h"
#include "PublicKeyIndex.h"

struct grdw_community_root;

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT CommunityRootTx 
  {
    uint32_t publicKeyIndex; // 4 Bytes
    uint32_t gmwPublicKeyIndex; // 4 Bytes
    uint32_t aufPublicKeyIndex; // 4 Bytes

    static CommunityRootTx fromGrdw(const grdw_community_root* grdw_community_root, uint32_t blockchainCommunityIdIndex);
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__