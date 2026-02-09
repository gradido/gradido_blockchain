#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__

#include "PublicKeyIndex.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT TransferTx 
  {
    int64_t amount; // 8 Bytes
    PublicKeyIndex senderPublicKeyIndex; // 8 Bytes
    PublicKeyIndex recipientPublicKeyIndex; // 8 Bytes

    inline bool isInvolved(PublicKeyIndex other) const {
      return 
        other.publicKeyIndex == senderPublicKeyIndex.publicKeyIndex || 
        other.publicKeyIndex == recipientPublicKeyIndex.publicKeyIndex;
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__