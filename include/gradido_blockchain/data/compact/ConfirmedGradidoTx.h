#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__  

#include "AccountBalance.h"
#include "CommunityRootTx.h"
#include "ConfirmedGradidoTxCold.h"
#include "CreationTx.h"
#include "DeferredTransferTx.h"
#include "PublicKeyIndex.h"
#include "RegisterAddressTx.h"
#include "RedeemDeferredTransferTx.h"
#include "TimeoutDeferredTransferTx.h"
#include "TransferTx.h"
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

#include <optional>
#include <set>

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedGradidoTx 
  {
    ConfirmedGradidoTx();
    ~ConfirmedGradidoTx();

    // copy constructor
    ConfirmedGradidoTx(const ConfirmedGradidoTx& other);
    // move
    ConfirmedGradidoTx(ConfirmedGradidoTx&& other);

    static ConfirmedGradidoTx fromGrdw(
      const grdw_confirmed_transaction* tx, 
      const grdw_transaction_body* body, 
      uint32_t blockchainCommunityIdIndex,
      bool loadColdData = true
    );

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
    // don't always exist!
    ConfirmedGradidoTxCold* coldData;

    // return only a value if one if the account balances has a different coin community id index as this blockchain
    // throw if more than one different coin community id index was found
    std::optional<uint32_t> getCoinCommunityId() const;
    // if cold isn't loaded, doesn't contain pubkeys from signature map
    std::set<PublicKeyIndex> getInvolvedAddresses() const;
    bool isBalanceUpdated(PublicKeyIndex pubkeyIndex) const;

    inline bool isTransfer() const { return TransactionType::TRANSFER == transactionType; }
    inline bool isCreation() const { return TransactionType::CREATION == transactionType; }
    inline bool isCommunityFriendsUpdate() const { return TransactionType::COMMUNITY_FRIENDS_UPDATE == transactionType; }
    inline bool isRegisterAddress() const { return TransactionType::REGISTER_ADDRESS == transactionType; }
    inline bool isDeferredTransfer() const { return TransactionType::DEFERRED_TRANSFER == transactionType; }
    inline bool isCommunityRoot() const { return TransactionType::COMMUNITY_ROOT == transactionType; }
    inline bool isRedeemDeferredTransfer() const { return TransactionType::REDEEM_DEFERRED_TRANSFER == transactionType; }
    inline bool isTimeoutDeferredTransfer() const { return TransactionType::TIMEOUT_DEFERRED_TRANSFER == transactionType; }
    inline bool isCrossCommunityTx() const { return CrossGroupType::LOCAL != crossGroupType; }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__