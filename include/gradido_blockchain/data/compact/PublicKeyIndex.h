#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/crypto/ByteArray.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT PublicKeyIndex 
  {
    static PublicKeyIndex fromPublicKey(uint32_t communityIdIndex, const PublicKey& publicKey);
    PublicKey getRawKey() const;
    uint32_t communityIdIndex;
    uint32_t publicKeyIndex;  

    inline bool operator==(const PublicKeyIndex& other) const {
      return communityIdIndex == other.communityIdIndex && publicKeyIndex == other.publicKeyIndex;
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H