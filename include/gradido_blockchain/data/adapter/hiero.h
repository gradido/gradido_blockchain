#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_HIERO_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_HIERO_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/compact/HieroTransactionId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido::data::adapter {
  inline hiero::TransactionId fromCompact(compact::HieroTransactionId compactTransactionId)
  {
    return hiero::TransactionId(
      Timestamp(compactTransactionId.seconds, compactTransactionId.nanos),
      hiero::AccountId(0, 0, compactTransactionId.accountNum)
    );
  }
  GRADIDOBLOCKCHAIN_EXPORT compact::HieroTransactionId toCompact(const hiero::TransactionId& tx);
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_HIERO_H