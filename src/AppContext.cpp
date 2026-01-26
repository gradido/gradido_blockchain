#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/Dictionary.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include "loguru/loguru.hpp"
#include <optional>
#include <regex>
#include <string>
#include <memory>

using std::optional, std::nullopt;
using std::regex, std::regex_match;
using std::string, std::to_string;
using std::shared_ptr, std::make_unique, std::unique_ptr;

namespace gradido {
  using data::compact::PublicKeyIndex;

  unique_ptr<AppContext> g_appContext = nullptr;
  regex regExCommunityAlias(COMMUNITY_ID_REGEX_STRING);

  AppContext::AppContext(
    unique_ptr<IMutableDictionary<string>> communityIds
  ) : mCommunityIds(std::move(communityIds))//, mPublicKeys(std::move(publicKeys))
  {

  }

  uint32_t AppContext::getOrAddCommunityIdIndex(const string& communityId)
  {
    if (!isValidCommunityAlias(communityId)) {
      // TODO: Custom exception types for this
      string error = "invalid character, only lowercase english latin letter, numbers and - are allowed for communityId: ";
      error += communityId;
      throw GradidoNodeInvalidDataException(error.c_str());      
    }
    size_t index = mCommunityIds->getOrAddIndexForData(communityId);
    if (static_cast<uint32_t>(index) != index) {
      LOG_F(FATAL, "more communities as expected, uint32_t don't is enough");
      throw GradidoNotImplementedException("communities with more then uint32_t index can handle isn't implemented");
    }
    if (mCommunityContexts.size() == index) {
      mCommunityContexts.emplace_back(communityId, static_cast<uint32_t>(index));
    }
    else if (mCommunityContexts.size() < index) {
      throw DictionaryHoleException("community contexts deque has a hole", "communityIds", mCommunityContexts.size(), index);
    }
    
    return index;
  }

  void AppContext::syncCommunityContextsWithCommunityIds()
  {
    size_t index = mCommunityContexts.size();

    while (auto data = mCommunityIds->getDataForIndex(index)) {
      mCommunityContexts.emplace_back(data.value(), static_cast<uint32_t>(index));
      ++index;
    }
  }

  void AppContext::addBlockchain(uint32_t communityIdIndex, shared_ptr<blockchain::Abstract> blockchain)
  {
    if (mCommunityContexts.size() <= communityIdIndex) {
      string entryName = to_string(communityIdIndex);
      auto communityId = mCommunityIds->getDataForIndex(communityIdIndex);      
      if (communityId) {
        entryName = communityId.value();
      }
      throw DictionaryMissingEntryException("missing CommunityContext", entryName);
    }
    mCommunityContexts[communityIdIndex].setBlockchain(blockchain);
  }

  uint32_t AppContext::addCommunity(const string& communityId, shared_ptr<blockchain::Abstract> blockchain)
  {
    auto index = getOrAddCommunityIdIndex(communityId);
    mCommunityContexts[index].setBlockchain(blockchain);
    return index;
  }

  optional<PublicKey> AppContext::getPublicKey(PublicKeyIndex index) const noexcept
  {
    if (!mCommunityIds || mCommunityContexts.size() <= index.communityIdIndex) {
      return nullopt;
    }
    const auto& blockchain = mCommunityContexts[index.communityIdIndex].getBlockchain();
    if (!blockchain) {
      return nullopt;
    }
    return blockchain->getPublicKeyDictionary().getDataForIndex(index.publicKeyIndex);
  }

  bool AppContext::hasPublicKey(data::compact::PublicKeyIndex index) const noexcept
  {
    if (!mCommunityIds || mCommunityContexts.size() <= index.communityIdIndex) {
      return false;
    }
    const auto& blockchain = mCommunityContexts[index.communityIdIndex].getBlockchain();
    if (!blockchain) {
      return false;
    }
    return blockchain->getPublicKeyDictionary().hasIndex(index.publicKeyIndex);
  }

  uint32_t AppContext::getOrAddPublicKeyIndex(uint32_t communityIdIndex, const PublicKey& publicKey)
  {
    if (!mCommunityIds || mCommunityContexts.size() <= communityIdIndex) {
      throw DictionaryMissingEntryException("missing community entry", to_string(communityIdIndex));
    }
    return mCommunityContexts[communityIdIndex].getOrAddPublicKey(publicKey);
  }

  bool AppContext::isValidCommunityAlias(const string& communityId) const
  {
    return regex_match(communityId.begin(), communityId.end(), regExCommunityAlias);
  }
}