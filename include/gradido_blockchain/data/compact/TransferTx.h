#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__

#include "gradido_blockchain_core/data/wire/specific_transactions.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT TransferTx 
  {
    int64_t amountGddCent; // 8 Bytes
    uint32_t senderPublicKeyIndex; // 8 Bytes
    uint32_t recipientPublicKeyIndex; // 8 Bytes

    inline static TransferTx fromGrdw(
      const grdw_gradido_transfer* tx_transfer, 
      uint32_t senderCommunityIdIndex, 
      uint32_t recipientCommunityIdIndex,
      AppContext& appContext
    )
    {
      return {
        .amountGddCent = tx_transfer->sender.amount,
        .senderPublicKeyIndex = appContext.getOrAddPublicKeyIndex(senderCommunityIdIndex, tx_transfer->sender.pubkey),
        .recipientPublicKeyIndex = appContext.getOrAddPublicKeyIndex(recipientCommunityIdIndex, tx_transfer->recipient)
      };
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__