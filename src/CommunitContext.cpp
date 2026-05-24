#include "gradido_blockchain/CommunityContext.h"
#include "gradido_blockchain/data/adapter/uuid.h"

#include "loguru/loguru.hpp"

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace gradido {
  using data::adapter::uuidFromString;

  CommunityContext::CommunityContext(const string& communityId, uint32_t communityIdIndex) 
  : mCommunityUuid(uuidFromString(communityId.c_str())), mCommunityIdIndex(communityIdIndex)
  {
    
  }

  CommunityContext::CommunityContext(const Uuid& communityuuid, uint32_t communityIdIndex)
    : mCommunityUuid(communityuuid), mCommunityIdIndex(communityIdIndex)
  {

  }

  uint32_t CommunityContext::getOrAddPublicKey(const PublicKey& publicKey)
  {
    if (!mBlockchain) {
      throw GradidoNodeInvalidDataException("blockchain ptr is empty");
    }
    if (publicKey.isEmpty()) {
      throw GradidoNodeInvalidDataException("empty public key");
    }
    return mBlockchain->getOrAddPublicKey(publicKey);
  }
}