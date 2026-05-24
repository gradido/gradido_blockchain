#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/data/ByteArray.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT PublicKeyIndex 
  {
    static PublicKeyIndex fromPublicKey(uint32_t communityIdIndex, const PublicKey& publicKey);
    // get raw key from app context->communityContext or exception when not found
    PublicKey getRawKey() const;
    // return a string, communityid . public key hex if found, or ids when not found
    std::string toString() const noexcept;

    inline bool empty() const { return !communityIdIndex && !publicKeyIndex; }

    uint32_t communityIdIndex;
    uint32_t publicKeyIndex;  

    bool operator==(const PublicKeyIndex&) const = default;
    bool operator!=(const PublicKeyIndex&) const = default;
    inline bool operator<(const PublicKeyIndex& other) const {
      if (communityIdIndex != other.communityIdIndex) {
        return communityIdIndex < other.communityIdIndex;
      }
      return publicKeyIndex < other.publicKeyIndex;
    };
  };

  struct GRADIDOBLOCKCHAIN_EXPORT PublicKeyIndexHash
  {
    size_t operator()(const PublicKeyIndex& s) const noexcept {
      uint64_t combined = (uint64_t)(s.communityIdIndex) << 32 | s.publicKeyIndex;
      return std::hash<uint64_t>{}(combined);
    }
  };

  struct GRADIDOBLOCKCHAIN_EXPORT PublicKeyIndexEqual
  {
    bool operator()(const PublicKeyIndex& a, const PublicKeyIndex& b) const noexcept {
      return a == b;
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H