#include "gradido_blockchain_core/data/wire/basic_types.h"
#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/adapter/ReturnType.h"
#include "gradido_blockchain/data/adapter/transactionBody.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoUnit.h"

#include <cassert>
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
      g_appContext->getOrAddCommunityIdIndex(Uuid(grdwAmount.community_uuid))
    );
  }

  GRADIDOBLOCKCHAIN_EXPORT grdw_transfer_amount toGrdw(grd_memory* alloc, const TransferAmount& amount, uint32_t communityIdIndex)
  {
    assert(amount.getPublicKey()->size() == 32);
    grdw_transfer_amount grdwAmount;
    memcpy(grdwAmount.pubkey, amount.getPublicKey()->data(), 32);
    grdwAmount.amount = amount.getAmount().getGradidoCent();
    auto const& coinCommunityUuid = g_appContext->getCommunityIds().getDataForIndex(amount.getCoinCommunityIdIndex());
    if (!coinCommunityUuid) {
      throw DictionaryMissingEntryException("missing community id", to_string(communityIdIndex));
    }
    assert(coinCommunityUuid->size() == 16);
    memcpy(grdwAmount.community_uuid, coinCommunityUuid->data(), coinCommunityUuid->size());

    return grdwAmount;
  }
}
