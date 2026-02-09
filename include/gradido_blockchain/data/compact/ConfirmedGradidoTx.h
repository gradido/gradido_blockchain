#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__  

#include "AccountBalance.h"
#include "CommunityRootTx.h"
#include "ConfirmedGradidoTxCold.h"
#include "CreationTx.h"
#include "DeferredTransferTx.h"
#include "TransferTx.h"
#include "RegisterAddressTx.h"
#include "RedeemDeferredTransferTx.h"
#include "TimeoutDeferredTransferTx.h"
#include "TxId.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/DurationSeconds.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TimestampSeconds.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedGradidoTx 
  {
    ConfirmedGradidoTx();
    ~ConfirmedGradidoTx();

    static ConfirmedGradidoTx fromGrdw(const grdw_confirmed_transaction* tx, const grdw_transaction_body* body, uint32_t blockchainCommunityIdIndex);

    // packed tx and timestamp together to save 8 Byte padding
    uint64_t txNr;
    uint64_t confirmedAtSeconds;
    uint32_t confirmedAtNanos;
    uint32_t txCommunityIdIndex;

    inline Timestamp getConfirmedAt() const { return Timestamp(confirmedAtSeconds, confirmedAtNanos); }
    inline TxId getTxId() const { return TxId(txNr, txCommunityIdIndex); }    
        
    // enums, usually uint8_t
    CrossGroupType crossGroupType;
    TransactionType transactionType;
    BalanceDerivationType balanceDerivationType;
    uint8_t accountBalanceCount;

    AccountBalance accountBalances[3];

    // common fields for most transactions
    union { // 24 Bytes
      CreationTx creation;
      TransferTx transfer;
      DeferredTransferTx deferredTransfer; // fund deferred transfer address only on your own community
      RedeemDeferredTransferTx* redeemDeferredTransfer; // ptr because RedeemDeferredTransferTx is 40 bytes rather than 24 Bytes
      TimeoutDeferredTransferTx timeoutDeferredTransfer;
      RegisterAddressTx registerAddress;
      CommunityRootTx communityRoot;
    } specific;        
    // cold data, not on hot path, should be used less frequently
    // always valid pointer, will created via constructor and free in deconstructor 
    ConfirmedGradidoTxCold* coldData;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__