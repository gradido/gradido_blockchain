#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"

namespace gradido::data::compact {
    PublicKeyIndex PublicKeyIndex::fromPublicKey(uint32_t communityIdIndex, const PublicKey& publicKey) {
      auto publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(communityIdIndex, publicKey);
      return { .communityIdIndex = communityIdIndex , .publicKeyIndex = publicKeyIndex };
    }
}