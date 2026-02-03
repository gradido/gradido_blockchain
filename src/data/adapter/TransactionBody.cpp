#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/adapter/ReturnType.h"
#include "gradido_blockchain/data/adapter/TransactionBody.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_protobuf_zig.h"

#include <memory>
#include <string>

using memory::Block;
using std::shared_ptr, std::make_shared, std::to_string;

namespace gradido::data::adapter {

  GRADIDOBLOCKCHAIN_EXPORT TransferAmount fromGrdw(const grdw_transfer_amount& grdwAmount, uint32_t communityIdIndex)
  {
    return TransferAmount(
      make_shared<Block>(32, grdwAmount.pubkey),
      GradidoUnit::fromGradidoCent(grdwAmount.amount),
      grdwAmount.community_id ? g_appContext->getOrAddCommunityIdIndex(grdwAmount.community_id) : communityIdIndex
    );
  }
  GRADIDOBLOCKCHAIN_EXPORT grdw_transfer_amount toGrdw(const TransferAmount& amount, uint32_t communityIdIndex)
  {
    assert(amount.getPublicKey()->size() == 32);
    grdw_transfer_amount grdwAmount;
    memcpy(grdwAmount.pubkey, amount.getPublicKey()->data(), 32);
    grdwAmount.amount = amount.getAmount().getGradidoCent();
    if (amount.getCoinCommunityIdIndex() == communityIdIndex) {
      grdwAmount.community_id = nullptr;
    }
    else {
      auto const& communityId = g_appContext->getCommunityIds().getDataForIndex(communityIdIndex);
      if (!communityId) {
        throw DictionaryMissingEntryException("missing community id", to_string(communityIdIndex));
      }
      grdwAmount.community_id = grdu_reserve_copy_string(communityId->data(), communityId->size());
    }
    return grdwAmount;
  }
}
