#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/adapter/accountBalance.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

using memory::Block;
using std::make_shared;

namespace gradido::data {
  namespace adapter {
    AccountBalance fromGrdw(const grdw_account_balance& grdwAccountBalance, uint32_t communityIdIndex) 
    {
      auto pubkeyPtr = make_shared<const Block>(32, grdwAccountBalance.pubkey);
      auto balance = GradidoUnit::fromGradidoCent(grdwAccountBalance.balance);
      if (grdwAccountBalance.community_id) {
        return AccountBalance(pubkeyPtr, balance, grdwAccountBalance.community_id);
      }
      else {
        return AccountBalance(pubkeyPtr, balance, communityIdIndex);
      }
    }
    grdw_account_balance toGrdw(const AccountBalance& grdwAccountBalance, uint32_t communityIdIndex)
    {
      grdw_account_balance result;
      result.balance = grdwAccountBalance.getBalance().getGradidoCent();
      assert(grdwAccountBalance.getPublicKey() && grdwAccountBalance.getPublicKey()->size() == 32);
      memcpy(result.pubkey, grdwAccountBalance.getPublicKey()->data(), 32);
      if (communityIdIndex != grdwAccountBalance.getCoinCommunityIdIndex()) {
        auto communityId = g_appContext->getCommunityIds().getDataForIndexOrThrow(grdwAccountBalance.getCoinCommunityIdIndex());
        // subtract one from size, because grdu_reserve_copy_string add a \0 nevertheless
        result.community_id = grdu_reserve_copy_string(communityId.data(), communityId.size() - 1);
      }
      else {
        result.community_id = nullptr;
      }
      return result;
    }
  }
}