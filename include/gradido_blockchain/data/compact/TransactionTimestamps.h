#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSACTION_TIMESTAMPS_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSACTION_TIMESTAMPS_H__

#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace gradido::data::compact {
  // for two timestamps, which are related to each other
  // createdAt is the first timestamp
  // confirmedAt is the second timestamp
  struct GRADIDOBLOCKCHAIN_EXPORT TransactionTimestamps 
  {
    uint64_t createdSeconds;
    uint32_t createdNanos;
    // hedera is a bit special, confirmedAt can be before createdAt
    int32_t deltaMs; // confirmedAt = createdAt + delta

    inline Timestamp getConfirmedAt() const {
      auto seconds = createdSeconds + deltaMs / 1000;
      auto nanos   = createdNanos + (deltaMs % 1000) * 1'000'000;
      return Timestamp(seconds, nanos);
    }

    inline Timestamp getCreatedAt() const {
      return Timestamp(createdSeconds, createdNanos);
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSACTION_TIMESTAMPS_H__