#include "gradido_blockchain/data/adapter/hiero.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido::data::adapter {

  compact::HieroTransactionId toCompact(const hiero::TransactionId& tx) {
    const auto& accountId = tx.getAccountId();
    if (accountId.getAlias() || accountId.getRealmNum() || accountId.getShardNum()) {
      throw GradidoNodeInvalidDataException("cannot use compact format for hiero transaction id, because account id contain alias, realm or shard");
    }
    if (tx.getNonce() || tx.isScheduled()) {
      throw GradidoNodeInvalidDataException("cannot use compact format for hiero transaction id, because nonce or scheduled is set");
    }
    if (accountId.getAccountNum() != static_cast<int32_t>(accountId.getAccountNum())) {
      throw GradidoNodeInvalidDataException("cannot use compact format for hiero transaction id, because accountNum exceed int32");
    }
    return {
      .seconds = tx.getTransactionValidStart().getSeconds(),
      .nanos = tx.getTransactionValidStart().getNanos(),
      .accountNum = static_cast<int32_t>(tx.getAccountId().getAccountNum())
    };
  }

  compact::HieroTransactionId fromGrdw(grdw_hiero_transaction_id& grdw_hiero_tx_id)
  {
    if (grdw_hiero_tx_id.accountID.realmNum
      || grdw_hiero_tx_id.accountID.shardNum 
      || static_cast<int32_t>(grdw_hiero_tx_id.accountID.accountNum) != grdw_hiero_tx_id.accountID.accountNum
    ) {
      throw GradidoNodeInvalidDataException("cannot convert hiero transaction id from grdw to compact");
    }
    return {
      .seconds = grdw_hiero_tx_id.transactionValidStart.seconds,
      .nanos = grdw_hiero_tx_id.transactionValidStart.nanos,
      .accountNum = static_cast<int32_t>(grdw_hiero_tx_id.accountID.accountNum)
    };
  }
}
