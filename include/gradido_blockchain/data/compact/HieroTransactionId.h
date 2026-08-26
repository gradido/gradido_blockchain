#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_HIERO_TRANSACTION_ID_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_HIERO_TRANSACTION_ID_H

#include "gradido_blockchain/types.h"

namespace gradido::data::compact {
  // only work as long as shard and realm are 0 and accountNum is less than 2^32
  struct HieroTransactionId 
  {
    // timestamp
    int64_t seconds;
    int32_t nanos;

    // account
    int32_t accountNum;    

    inline bool operator==(const HieroTransactionId& other) const {
      return seconds == other.seconds && nanos == other.nanos && accountNum == other.accountNum;
    }
  };
} 

namespace std {
  template <>
  struct hash<gradido::data::compact::HieroTransactionId> {
    std::size_t operator()(const gradido::data::compact::HieroTransactionId& tid) const noexcept {
      return tid.seconds;
    }
  };
}


#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_HIERO_TRANSACTION_ID_H