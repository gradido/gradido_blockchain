#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include <string>
using std::string, std::to_string;

namespace gradido::data::compact {
    PublicKeyIndex PublicKeyIndex::fromPublicKey(uint32_t communityIdIndex, const PublicKey& publicKey) {
      auto publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(communityIdIndex, publicKey);
      return { .communityIdIndex = communityIdIndex , .publicKeyIndex = publicKeyIndex };
    }

    PublicKey PublicKeyIndex::getRawKey() const
    {
      auto rawPublicKey = g_appContext->getPublicKey(*this);
      if (!rawPublicKey) {
        string entryName;
        auto communityId = g_appContext->getCommunityIds().getDataForIndex(communityIdIndex);
        if (communityId) {
          entryName = communityId.value();
        }
        else {
          entryName = to_string(communityIdIndex);
        }
        entryName += '.';
        entryName += to_string(publicKeyIndex);
        throw DictionaryMissingEntryException("cannot find public key by PublicKeyIndex", entryName.c_str());
      }
      return rawPublicKey.value();
    }

    string PublicKeyIndex::toString() const noexcept
    {
      string result;
      auto communityId = g_appContext->getCommunityIds().getDataForIndex(communityIdIndex);
      if (communityId) {
        result = communityId.value();
      }
      else {
        result = to_string(communityIdIndex);
      }
      result += '.';

      auto rawPublicKey = g_appContext->getPublicKey(*this);
      if (!rawPublicKey) {
        result += to_string(publicKeyIndex);
      }
      else {
        result += rawPublicKey->convertToHex();
      }
      return result;
    }
}