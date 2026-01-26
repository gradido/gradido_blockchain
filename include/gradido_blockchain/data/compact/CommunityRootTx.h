#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT CommunityRootTx 
  {
    PublicKeyIndex publicKeyIndex; // 8 Bytes
    PublicKeyIndex gmwPublicKeyIndex; // 8 Bytes
    PublicKeyIndex aufPublicKeyIndex; // 8 Bytes

    inline bool isInvolved(PublicKeyIndex other) const {
      if (other.communityIdIndex != publicKeyIndex.communityIdIndex) {
        return false;
      }
      return
        other.publicKeyIndex == publicKeyIndex.publicKeyIndex ||
        other.publicKeyIndex == gmwPublicKeyIndex.publicKeyIndex ||
        other.publicKeyIndex == aufPublicKeyIndex.publicKeyIndex;
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__