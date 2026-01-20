#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/Dictionary.h"

#include "loguru/loguru.hpp"
#include <regex>
#include <memory>

using std::regex, std::regex_match;
using std::make_unique, std::unique_ptr;

namespace gradido {
  unique_ptr<AppContext> g_appContext = nullptr;
  regex regExCommunityAlias(COMMUNITY_ID_REGEX_STRING);

  AppContext::AppContext(
    std::unique_ptr<IMutableDictionary<std::string>> communityIds
    // std::unique_ptr<IMutableDictionary<memory::ConstBlockPtr>> publicKeys
  ) : mCommunityIds(std::move(communityIds))//, mPublicKeys(std::move(publicKeys))
  {

  }

  uint32_t AppContext::getOrAddCommunityIdIndex(const std::string& communityId)
  {
    // return 0;
    if (!isValidCommunityAlias(communityId)) {
      // TODO: Custom exception types for this
      std::string error = "invalid character, only lowercase english latin letter, numbers and - are allowed for communityId: ";
      error += communityId;
      throw GradidoNodeInvalidDataException(error.c_str());      
    }
    size_t index = mCommunityIds->getOrAddIndexForData(communityId);
    if (static_cast<uint32_t>(index) != index) {
      LOG_F(FATAL, "more communities as expected, uint32_t don't is enough");
      throw GradidoNotImplementedException("communities with more then uint32_t index can handle isn't implemented");
    }
    return index;
  }

  bool AppContext::isValidCommunityAlias(const std::string& communityId) const
  {
    return regex_match(communityId.begin(), communityId.end(), regExCommunityAlias);
  }
}