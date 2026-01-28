#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REGISTER_ADDRESS_TX_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REGISTER_ADDRESS_TX_H

#include "PublicKeyIndex.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT RegisterAddressTx 
  {
    AddressType addressType; // 1 Byte
    uint16_t derivationIndex; // 2 Byte (for the time beeing, update if more than 65535 are needed) 
    uint32_t nameHashIndex; // 4 Bytes
    PublicKeyIndex userPublicKeyIndex; // 8 Bytes
    PublicKeyIndex accountPublicKeyIndex; // 8 Bytes

    bool operator==(const RegisterAddressTx&) const = default;
    inline bool isInvolved(PublicKeyIndex publicKey) const {
      return userPublicKeyIndex == publicKey || accountPublicKeyIndex == publicKey;
    }
  };
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_COMPACT_REGISTER_ADDRESS_TX_H