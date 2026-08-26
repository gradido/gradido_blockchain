#include "gradido_blockchain_core/data/wire/specific_transactions.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"

namespace gradido::data::compact {
  CommunityRootTx CommunityRootTx::fromGrdw(const grdw_community_root* grdw_community_root, uint32_t blockchainCommunityIdIndex)
  {
    CommunityRootTx communityRootTx;
    communityRootTx.publicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_community_root->pubkey).publicKeyIndex;
    communityRootTx.gmwPublicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_community_root->gmw_pubkey).publicKeyIndex;
    communityRootTx.aufPublicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_community_root->auf_pubkey).publicKeyIndex;
    return communityRootTx;
  }
}