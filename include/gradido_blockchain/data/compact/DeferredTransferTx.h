#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_DEFERRED_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_DEFERRED_TRANSFER_TX_H__

#include "PublicKeyIndex.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/data/DurationSeconds.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT DeferredTransferTx 
  {
    uint32_t amountGddCent; // 8 Bytes
    // work only on local, take communityIdIndex from txId
    uint32_t senderPublicKeyIndex; // 4 Bytes
    uint32_t recipientPublicKeyIndex; // 4 Bytes
    uint32_t timeoutDurationSeconds; // 4 Bytes

    inline bool isInvolved(PublicKeyIndex other) const {
      return 
        other.publicKeyIndex == senderPublicKeyIndex || 
        other.publicKeyIndex == recipientPublicKeyIndex;
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_DEFERRED_TRANSFER_TX_H__