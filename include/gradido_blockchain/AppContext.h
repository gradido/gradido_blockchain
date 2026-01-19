#ifndef __GRADIDO_BLOCKCHAIN_APP_CONTEXT__
#define __GRADIDO_BLOCKCHAIN_APP_CONTEXT__

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"

#include <memory>
#include <string>

namespace memory {
  class Block;
  using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
  class GRADIDOBLOCKCHAIN_EXPORT AppContext
  {
  public:
    AppContext(
      std::unique_ptr<IMutableDictionary<std::string>> communityIds
      // std::unique_ptr<IMutableDictionary<memory::ConstBlockPtr>> publicKeys
    );
    uint32_t addCommunityId(const std::string& communityId);
    inline const IDictionary<std::string>& getCommunityIds() const { return *mCommunityIds; }
    // inline IMutableDictionary<memory::ConstBlockPtr>& getPublicKeys() { return *mPublicKeys; }
  protected:
    bool isValidCommunityAlias(const std::string& communityId) const;
    std::unique_ptr<IMutableDictionary<std::string>> mCommunityIds;
    // std::unique_ptr<IMutableDictionary<memory::ConstBlockPtr>> mPublicKeys;
  };

  GRADIDOBLOCKCHAIN_EXPORT extern std::unique_ptr<AppContext> g_appContext;
}

#endif // __GRADIDO_BLOCKCHAIN_APP_CONTEXT__