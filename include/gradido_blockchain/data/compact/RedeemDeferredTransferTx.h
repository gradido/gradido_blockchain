#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__

#include "TransferTx.h"
#include "TxId.h"
#include "AccountBalance.h"
#include "gradido_blockchain/export.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT RedeemDeferredTransferTx 
  {
    // return ptr must be deletes after not longer need
    // todo: think about using std::unique_ptr
    inline static RedeemDeferredTransferTx* fromGrdw(
      const grdw_gradido_redeem_deferred_transfer* redeem_deferred_transfer,
      uint32_t senderCommunityIdIndex,
      uint32_t recipientCommunityIdIndex
    ) {
      return new RedeemDeferredTransferTx{
        .deferredTransferTransactionNr = {
          .nr = redeem_deferred_transfer->deferred_transfer_transaction_nr,
          .communityIdIndex = senderCommunityIdIndex
        },
        .transfer = TransferTx::fromGrdw(&redeem_deferred_transfer->transfer, senderCommunityIdIndex, recipientCommunityIdIndex)
      };
    }
    TxId deferredTransferTransactionNr;
    TransferTx transfer;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__