#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data::compact {
  CommunityRootTx CommunityRootTx::fromGrdw(const grdw_community_root* grdw_community_root, uint32_t blockchainCommunityIdIndex)
  {
    CommunityRootTx communityRootTx;
    communityRootTx.publicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_community_root->pubkey);
    communityRootTx.gmwPublicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_community_root->gmw_pubkey);
    communityRootTx.aufPublicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_community_root->auf_pubkey);
    return communityRootTx;
  }
}