#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__

#include "TransferTx.h"
#include "TxId.h"
#include "AccountBalance.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::compact {
    struct GRADIDOBLOCKCHAIN_EXPORT RedeemDeferredTransferTx 
    {
        TxId deferredTransferTransactionNr;
        TransferTx transfer;
    };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__