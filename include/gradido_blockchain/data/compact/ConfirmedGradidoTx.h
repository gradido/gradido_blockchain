#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__  

#include "AccountBalance.h"
#include "CommunityRootTx.h"
#include "PublicKeyIndex.h"
#include "TransactionTimestamps.h"
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



namespace gradido::data::compact {
    struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedGradidoTx 
    {
        TransactionTimestamps timestamps;

        inline Timestamp getConfirmedAt() const { return timestamps.getConfirmedAt(); }
        inline Timestamp getCreatedAt() const { return timestamps.getCreatedAt(); }

        // txId and pairingTxId packed to save 8 Bytes padding
        uint64_t txNrs[2];
        uint32_t communityIdIndex[2];
        inline TxId getTxId() const { return TxId(txNrs[0], communityIdIndex[0]); }
        // for cross group transactions, else empty
        inline TxId getPairingTxId() const { return TxId(txNrs[1], communityIdIndex[1]); }
        
        // memos and public key/ signature pairs via txId in separate list/manager/thingy
        
        // enums, usually uint8_t
        CrossGroupType crossGroupType;
        TransactionType transactionType;
        BalanceDerivationType balanceDerivationType;
        uint8_t accountBalanceCount;

        AccountBalance accountBalances[2];

        // common fields for most transactions
        union { // 24 Bytes
          struct {
            GradidoUnit amount; // 8 Bytes
            PublicKeyIndex recipientPublicKeyIndex; // 8 Bytes
            TimestampSeconds targetDate; // 8 Bytes
          } creation;
          struct {
            GradidoUnit amount; // 8 Bytes
            PublicKeyIndex senderPublicKeyIndex; // 8 Bytes
            PublicKeyIndex recipientPublicKeyIndex; // 8 Bytes
          } transfer; // also used for redeem deferred transfer, and deferredTransferTransactionNr is stored in extra dictionary
          struct {
            GradidoUnit amount; // 8 Bytes
            // work only on local, take communityIdIndex from txId
            uint32_t senderPublicKeyIndex; // 4 Bytes
            uint32_t recipientPublicKeyIndex; // 4 Bytes
            DurationSeconds timeoutDuration; // 4 Bytes
          } deferredTransfer; // fund deferred transfer address only on your own community
          struct {
            TxId deferredTransferTransactionNr; // 16 Bytes, contain 4 Bytes padding
          } timeoutDeferredTransfer;
          struct {
            AddressType addressType; // 1 Byte
            uint16_t derivationIndex; // 2 Byte (for the time beeing, update if more than 65535 are needed) 
            uint32_t nameHashIndex; // 4 Bytes
            PublicKeyIndex userPublicKeyIndex; // 8 Bytes
            PublicKeyIndex accountPublicKeyIndex; // 8 Bytes
          } registerAddress;
          CommunityRootTx communityRoot;
        };        
    };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__