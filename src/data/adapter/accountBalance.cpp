#include "gradido_blockchain_core/data/wire/basic_types.h"
#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/AccountBalance.h"
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

      return AccountBalance(pubkeyPtr, balance, Uuid(grdwAccountBalance.community_uuid));
    }
    grdw_account_balance toGrdw(grd_memory* alloc, const AccountBalance& grdwAccountBalance, uint32_t communityIdIndex)
    {
      grdw_account_balance result;
      result.balance = grdwAccountBalance.getBalance().getGradidoCent();
      assert(grdwAccountBalance.getPublicKey() && grdwAccountBalance.getPublicKey()->size() == 32);
      memcpy(result.pubkey, grdwAccountBalance.getPublicKey()->data(), 32);
      
      auto communityId = g_appContext->getCommunityIds().getDataForIndexOrThrow(grdwAccountBalance.getCoinCommunityIdIndex());
      memcpy(result.community_uuid, communityId.data(), communityId.size());

      return result;
    }
  }
}