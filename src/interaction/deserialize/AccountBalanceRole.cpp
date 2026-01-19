#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/interaction/deserialize/AccountBalanceRole.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include <string>

using std::string;

namespace gradido {
  namespace interaction {
    namespace deserialize {
      AccountBalanceRole::AccountBalanceRole(const AccountBalanceMessage& accountBalance, uint32_t communityIdIndex)
      {
        string communityIdString = accountBalance["community_id"_f].value_or("");
        uint32_t coinCommunityIdIndex = communityIdIndex;
        if (!communityIdString.empty()) {
          auto coinCommunityIdIndexOptional = g_appContext->getCommunityIds().getIndexForData(communityIdString);
          if (!coinCommunityIdIndexOptional.has_value()) {
            throw DictionaryMissingEntryException(
              "missing community id for index in deserialize::AccountBalanceRole",
              communityIdString
            );
          }
          coinCommunityIdIndex = coinCommunityIdIndexOptional.value();
        }
        mAccountBalance = {
          std::make_shared<memory::Block>(accountBalance["pubkey"_f].value()),
          GradidoUnit::fromGradidoCent(accountBalance["balance"_f].value_or(0)),
          coinCommunityIdIndex
        };
      }
    }
  }
}
