#ifndef GRADIDO_BLOCKCHAIN_BLOCKCHAIN_BATCH_SIGNATURES_VERIFY_H
#define GRADIDO_BLOCKCHAIN_BLOCKCHAIN_BATCH_SIGNATURES_VERIFY_H

#include "gradido_blockchain/blockchain/batch/ThreadingPolicy.h"
#include "gradido_blockchain/data/ByteArray.h"
#include "gradido_blockchain/export.h"

#include <vector>

namespace gradido::blockchain {
  class Filter;
  class Abstract;
  
  namespace batch {
    GRADIDOBLOCKCHAIN_EXPORT std::vector<uint64_t> verifySignatures(const Filter& filter, const std::string& communityId, ThreadingPolicy policy = ThreadingPolicy::All);
    GRADIDOBLOCKCHAIN_EXPORT std::vector<uint64_t> verifySignatures(const Filter& filter, const data::Uuid& communityId, ThreadingPolicy policy = ThreadingPolicy::All);
  }
}

#endif // GRADIDO_BLOCKCHAIN_BLOCKCHAIN_BATCH_SIGNATURES_VERIFY_H