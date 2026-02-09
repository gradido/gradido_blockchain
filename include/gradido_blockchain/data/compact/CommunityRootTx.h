#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_COMMUNITY_ROOT_TX_H__

#include "gradido_blockchain/export.h"
#include "PublicKeyIndex.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT CommunityRootTx 
  {
    PublicKeyIndex publicKeyIndex; // 8 Bytes
    PublicKeyIndex gmwPublicKeyIndex; // 8 Bytes
    PublicKeyIndex aufPublicKeyIndex; // 8 Bytes

    static CommunityRootTx fromGrdw(const grdw_community_root* grdw_community_root, uint32_t blockchainCommunityIdIndex);

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