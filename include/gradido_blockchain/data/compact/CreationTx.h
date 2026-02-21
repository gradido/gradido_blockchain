#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CREATION_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CREATION_TX_H__

#include "PublicKeyIndex.h"
#include "gradido_blockchain/data/TimestampSeconds.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"

#include "date/date.h"

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT CreationTx 
  {
    int64_t amountGddCent; // 8 Bytes
    uint32_t recipientPublicKeyIndex; // 8 Bytes
    date::year_month targetDateSeconds; // 4 Bytes
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CREATION_TX_H__