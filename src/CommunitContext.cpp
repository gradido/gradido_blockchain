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

  void CommunityContext::setBlockchain(shared_ptr<blockchain::Abstract> blockchain)
  {
    if (mBlockchain) 
    {
      if (mBlockchain == blockchain) {
        LOG_F(WARNING, "blockchain for community %s was already set, pointer a identical", mCommunityId.c_str());
      }
      else {
        LOG_F(WARNING, "blockchain for community %s was already set, will be overwritten", mCommunityId.c_str());
        
      }
    }
    mBlockchain = blockchain;
  }

  uint32_t CommunityContext::getOrAddPublicKey(const PublicKey& publicKey)
  {
    if (!mBlockchain) {
      throw GradidoNodeInvalidDataException("blockchain ptr is empty");
    }
    return mBlockchain->getOrAddPublicKey(publicKey);
  }
}