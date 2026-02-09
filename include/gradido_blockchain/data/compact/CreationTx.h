#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CREATION_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CREATION_TX_H__

#include "PublicKeyIndex.h"
#include "gradido_blockchain/data/TimestampSeconds.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT CreationTx 
  {
    int64_t amountGddCent; // 8 Bytes
    PublicKeyIndex recipientPublicKeyIndex; // 8 Bytes
    int64_t targetDateSeconds; // 8 Bytes

    inline bool isInvolved(PublicKeyIndex other) const {
      return other.publicKeyIndex == recipientPublicKeyIndex.publicKeyIndex;
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CREATION_TX_H__