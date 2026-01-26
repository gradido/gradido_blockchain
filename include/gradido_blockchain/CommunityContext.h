#ifndef __GRADIDO_BLOCKCHAIN_COMMUNITY_CONTEXT_H__
#define __GRADIDO_BLOCKCHAIN_COMMUNITY_CONTEXT_H__

#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

#include <string>
#include <memory>

namespace gradido {
  class GRADIDOBLOCKCHAIN_EXPORT CommunityContext
  {
  public:
    CommunityContext(const std::string& communityId, uint32_t communityIdIndex);
    ~CommunityContext() = default;
    void setBlockchain(std::shared_ptr<blockchain::Abstract> blockchain);
    inline const std::shared_ptr<blockchain::Abstract>& getBlockchain() const { return mBlockchain; }
    inline const std::string& getCommunityId() const { return mCommunityId; }
    inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
    uint32_t getOrAddPublicKey(const PublicKey& publicKey);
  protected:
    std::string mCommunityId;
    uint32_t mCommunityIdIndex;
    std::shared_ptr<blockchain::Abstract> mBlockchain;
  };  
}

#endif // __GRADIDO_BLOCKCHAIN_COMMUNITY_CONTEXT_H__