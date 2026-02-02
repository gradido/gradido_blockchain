#include "gradido_blockchain/data/adapter/hiero.h"

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
}
