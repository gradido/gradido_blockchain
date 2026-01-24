#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__

#include "TxId.h"
#include "AccountBalance.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::compact {
    struct GRADIDOBLOCKCHAIN_EXPORT RedeemDeferredTransferTx {
        TxId deferredTransferTransactionNr;
        AccountBalance changeAccountBalance;
    };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REDEEM_DEFERRED_TRANSFER_TX_H__