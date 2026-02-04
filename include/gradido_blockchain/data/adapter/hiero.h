#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_HIERO_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_HIERO_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/compact/HieroTransactionId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data::adapter {
  inline hiero::TransactionId fromCompact(compact::HieroTransactionId compactTransactionId)
  {
    return hiero::TransactionId(
      Timestamp(compactTransactionId.seconds, compactTransactionId.nanos),
      hiero::AccountId(0, 0, compactTransactionId.accountNum)
    );
  }
  GRADIDOBLOCKCHAIN_EXPORT compact::HieroTransactionId toCompact(const hiero::TransactionId& tx);

  GRADIDOBLOCKCHAIN_EXPORT compact::HieroTransactionId fromGrdw(grdw_hiero_transaction_id& grdw_hiero_tx_id);
  inline grdw_hiero_transaction_id toGrdw(const compact::HieroTransactionId& hieroTxIdCompact) {
    return {
     .transactionValidStart = {.seconds = hieroTxIdCompact.seconds, .nanos = hieroTxIdCompact.nanos },
     .accountID = {.shardNum = 0,  .realmNum = 0, .accountNum = hieroTxIdCompact.accountNum }
    };
  }

}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_HIERO_H