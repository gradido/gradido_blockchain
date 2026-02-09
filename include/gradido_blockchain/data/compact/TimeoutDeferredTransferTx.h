#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TIMEOUT_DEFERRED_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TIMEOUT_DEFERRED_TRANSFER_TX_H__

#include "TxId.h"
#include "gradido_blockchain/export.h"


namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT TimeoutDeferredTransferTx 
  {
     TxId deferredTransferTransactionNr;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TIMEOUT_DEFERRED_TRANSFER_TX_H__