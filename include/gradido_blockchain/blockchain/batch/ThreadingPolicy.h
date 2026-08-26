#ifndef GRADIDO_BLOCKCHAIN_BLOCKCHAIN_BATCH_THREADING_POLICY_H
#define GRADIDO_BLOCKCHAIN_BLOCKCHAIN_BATCH_THREADING_POLICY_H

#include "gradido_blockchain/export.h"

#include <cstdint>
#include <cstddef>

namespace gradido::blockchain::batch {
  enum class ThreadingPolicy : uint8_t {
    SingleThread,
    Quarter,
    Half,
    ThreeQuarter,
    All,
    AllExceptOne
  };

  GRADIDOBLOCKCHAIN_EXPORT std::size_t resolveThreadCount(ThreadingPolicy policy);
}

#endif // GRADIDO_BLOCKCHAIN_BLOCKCHAIN_BATCH_THREADING_POLICY_H