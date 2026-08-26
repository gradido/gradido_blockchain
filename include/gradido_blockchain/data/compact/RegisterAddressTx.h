#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REGISTER_ADDRESS_TX_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_REGISTER_ADDRESS_TX_H

#include "PublicKeyIndex.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain_core/types/address.h"

struct grdw_register_address;

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT RegisterAddressTx 
  {
    grdt_address addressType; // 1 Byte
    uint16_t derivationIndex; // 2 Byte (for the time beeing, update if more than 65535 are needed) 
    uint32_t nameHashIndex; // 4 Bytes
    uint32_t userPublicKeyIndex; // 8 Bytes
    uint32_t accountPublicKeyIndex; // 8 Bytes

    static RegisterAddressTx fromGrdw(const grdw_register_address* grdw_register_address, uint32_t blockchainCommunityIdIndex);

    bool operator==(const RegisterAddressTx&) const = default;
  };
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_COMPACT_REGISTER_ADDRESS_TX_H