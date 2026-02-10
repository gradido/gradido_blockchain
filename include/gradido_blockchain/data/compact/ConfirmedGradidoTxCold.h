#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_COLD_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_COLD_H__  

#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/compact/TxId.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/types.h"
#include "gradido_protobuf_zig.h"

#include <optional>
#include <vector>

namespace gradido::data::compact {
  typedef std::pair<PublicKey, Signature> PublicKeySignaturePair;

  struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedGradidoTxCold 
  {
    ConfirmedGradidoTxCold();
    static ConfirmedGradidoTxCold* fromGrdw(
      const grdw_confirmed_transaction* tx,
      const grdw_transaction_body* body,
      uint32_t blockchainCommunityIdIndex
    );

    // packed tx and timestamp together to save 8 Byte padding
    uint64_t createdAtSeconds;
    uint32_t createdAtNanos;
    uint32_t pairingTxCommunityIdIndex;

    inline Timestamp getCreatedAt() const { return Timestamp(createdAtSeconds, createdAtNanos); }

    // memos
    // check if empty before use
    EncryptedMemo encryptedMemos[2];
     
    // pubkey - signature pairs
    std::vector<PublicKeySignaturePair> signatureMap;

    // running hash of confirmed transaction 
    GenericHash runningHash;

    // id on ledger
    LedgerAnchor ledgerAnchor;
    // by cross group transactions, pairing transaction on other blockchain
    LedgerAnchor pairingLedgerAnchor;

    // body bytes serialized 
    // because it is used by signature and protobuf serialization isn't deterministic, we need to save the original 
    memory::Block bodyBytes;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_COLD_H__