#ifndef __GRADIDO_BLOCKCHAIN_APP_CONTEXT__
#define __GRADIDO_BLOCKCHAIN_APP_CONTEXT__

#include "CommunityContext.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include <deque>
#include <memory>
#include <optional>
#include <string>

namespace gradido {

  class GRADIDOBLOCKCHAIN_EXPORT AppContext
  {
  public:
    AppContext(
      std::unique_ptr<IMutableDictionary<std::string>> communityIds,
      std::unique_ptr<IMutableDictionary<GenericHash>> userNameHashs 
    );
    uint32_t getOrAddCommunityIdIndex(const std::string& communityId);
    uint32_t getOrAddUserNameHashIndex(const GenericHash& hash);
    inline const IDictionary<std::string>& getCommunityIds() const { return *mCommunityIds; }
    inline const IDictionary<GenericHash>& getUseNameHashs() const { return *mUserNameHashs; }
    inline const CommunityContext& getCommunityContext(uint32_t communityIdIndex) const;
    // call getDataForIndex on mCommunityIds start by mCommunityContexts.size() until empty to synchronize mCommunityContexts with mCommunityIds
    // should be used on startup if IDictionary is a persistent Dictionary after loading entries from persistent storage
    void syncCommunityContextsWithCommunityIds();
    // add blockchain to existing community context entry
    void addBlockchain(uint32_t communityIdIndex, std::shared_ptr<blockchain::Abstract> blockchain);
    // create new community context entry (if community not exist) and set blockchain
    uint32_t addCommunity(const std::string& communityId, std::shared_ptr<blockchain::Abstract> blockchain);

    /**
   * @brief Get public key by combined public key index.
   *
   * The index consists of:
   * - community id index
   * - public key index
   *
   * @details
   * The function:
   * - accesses the corresponding community context in @c mCommunityContexts
   * - queries the blockchain associated with that community
   * - retrieves the public key from the blockchain's public key dictionary
   *
   * @return std::optional<PublicKey>
   *         - valid instance if the key exists
   *         - std::nullopt if no key can be retrieved
   *
   * @plantuml
   * sequenceDiagram
   *     participant Caller
   *     participant AppContext
   *     participant CommunityContext
   *     participant Blockchain
   *     participant PublicKeyDict
   *
   *     Caller ->> AppContext: getPublicKey(index)
   *     AppContext ->> CommunityContext: resolve communityIdIndex
   *     CommunityContext ->> Blockchain: getBlockchain()
   *     Blockchain ->> PublicKeyDict: lookup publicKeyIndex
   *
   *     alt key found
   *         PublicKeyDict -->> Blockchain: PublicKey
   *         Blockchain -->> AppContext: PublicKey
   *         AppContext -->> Caller: PublicKey
   *     else key not found
   *         PublicKeyDict -->> Blockchain: null
   *         Blockchain -->> AppContext: nullopt
   *         AppContext -->> Caller: nullopt
   *     end
   * @enduml
   */
    std::optional<PublicKey> getPublicKey(data::compact::PublicKeyIndex index) const noexcept;

    bool hasPublicKey(data::compact::PublicKeyIndex index) const noexcept;
    uint32_t getOrAddPublicKeyIndex(uint32_t communityIdIndex, const PublicKey& publicKey);
  protected:
    bool isValidCommunityAlias(const std::string& communityId) const;
    std::unique_ptr<IMutableDictionary<std::string>> mCommunityIds;
    std::unique_ptr<IMutableDictionary<GenericHash>> mUserNameHashs;
    // use communityIdIndex as index, start with 0, shouldn't contain holes
    std::deque<CommunityContext> mCommunityContexts;
  };

  const CommunityContext& AppContext::getCommunityContext(uint32_t communityIdIndex) const
  {
    if (communityIdIndex >= mCommunityContexts.size()) {
      throw DictionaryMissingEntryException("missing community context", std::to_string(communityIdIndex));
    }
    return mCommunityContexts[communityIdIndex];
  }

  GRADIDOBLOCKCHAIN_EXPORT extern std::unique_ptr<AppContext> g_appContext;
}

#endif // __GRADIDO_BLOCKCHAIN_APP_CONTEXT__