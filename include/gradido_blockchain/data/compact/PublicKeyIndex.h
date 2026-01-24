#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace gradido::data::compact {
    struct GRADIDOBLOCKCHAIN_EXPORT PublicKeyIndex {
        uint32_t publicKeyIndex;
        uint32_t communityIdIndex;
    };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_PUBLIC_KEY_INDEX_H