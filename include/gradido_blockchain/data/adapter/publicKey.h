#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_PUBLIC_KEY_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_PUBLIC_KEY_H

#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/memory/Block.h"

namespace gradido::data::adapter {
  inline PublicKey toPublicKey(const memory::Block& block) 
  {
    if (block.size() != 32) {
      throw GradidoNodeInvalidDataException("toPublicKey called with wrong sized block");
    }
    return PublicKey(block.data());
  }

  inline PublicKey toPublicKey(memory::ConstBlockPtr block)
  {
    return toPublicKey(*block);
  }
  
  GRADIDOBLOCKCHAIN_EXPORT memory::ConstBlockPtr toConstBlockPtr(compact::PublicKeyIndex publicKeyIndex);
  GRADIDOBLOCKCHAIN_EXPORT compact::PublicKeyIndex toPublicKeyIndex(const memory::Block& block, const std::string& communityId);
  GRADIDOBLOCKCHAIN_EXPORT compact::PublicKeyIndex toPublicKeyIndex(const PublicKey& publicKey, uint32_t communityIdIndex);

  inline compact::PublicKeyIndex toPublicKeyIndex(memory::ConstBlockPtr blockPtr, const std::string& communityId) {
    return toPublicKeyIndex(*blockPtr, communityId);
  }

  inline compact::PublicKeyIndex toPublicKeyIndex(const memory::Block& block, uint32_t communityIdIndex) {
    auto publicKey = toPublicKey(block);
    return toPublicKeyIndex(publicKey, communityIdIndex);
  }
  inline compact::PublicKeyIndex toPublicKeyIndex(memory::ConstBlockPtr blockPtr, uint32_t communityIdIndex) {
    return toPublicKeyIndex(*blockPtr, communityIdIndex);
  }
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_ADAPTER_PUBLIC_KEY_H