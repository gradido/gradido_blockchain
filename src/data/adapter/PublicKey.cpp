#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/adapter/ArrayBuffer.h"
#include "gradido_blockchain/data/adapter/PublicKey.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include <string>

using memory::Block, memory::ConstBlockPtr;
using std::string, std::to_string;

namespace gradido::data::adapter {
  using compact::PublicKeyIndex;

  ConstBlockPtr toConstBlockPtr(PublicKeyIndex publicKeyIndex)
  {
    auto result = g_appContext->getPublicKey(publicKeyIndex);
    if (!result) {
      string keyString;
      auto communityId = g_appContext->getCommunityIds().getDataForIndex(publicKeyIndex.communityIdIndex);
      if (communityId) {
        keyString = communityId.value();
      }
      else {
        keyString = to_string(publicKeyIndex.communityIdIndex);
      }
      keyString += '.';
      keyString = to_string(publicKeyIndex.publicKeyIndex);
      throw DictionaryMissingEntryException("missing public key in app context, in adapter", keyString.c_str());
    }
    return toConstBlockPtr(result.value());
  }

  PublicKeyIndex toPublicKeyIndex(const memory::Block& block, const string& communityId)
  {
    return toPublicKeyIndex(block, g_appContext->getOrAddCommunityIdIndex(communityId));
  }

  PublicKeyIndex toPublicKeyIndex(const PublicKey& publicKey, uint32_t communityIdIndex)
  {
    return {
      .communityIdIndex = communityIdIndex,
      .publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(communityIdIndex, publicKey)
    };
  }

}
