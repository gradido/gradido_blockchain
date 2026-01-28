#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/crypto/ByteArray.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT PublicKeyIndex 
  {
    static PublicKeyIndex fromPublicKey(uint32_t communityIdIndex, const PublicKey& publicKey);
    // get raw key from app context->communityContext or exception when not found
    PublicKey getRawKey() const;
    // return a string, communityid . public key hex if found, or ids when not found
    std::string toString() const noexcept;

    uint32_t communityIdIndex;
    uint32_t publicKeyIndex;  

    inline bool operator==(const PublicKeyIndex&) const = default;
    inline bool operator!=(const PublicKeyIndex&) const = default;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H