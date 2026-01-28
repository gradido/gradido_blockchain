#include "gradido_blockchain/CommunityContext.h"

#include "loguru/loguru.hpp"

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace gradido {
  CommunityContext::CommunityContext(const string& communityId, uint32_t communityIdIndex) 
  : mCommunityId(communityId), mCommunityIdIndex(communityIdIndex) 
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