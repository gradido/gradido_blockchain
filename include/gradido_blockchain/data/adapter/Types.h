#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H

#include "gradido_protobuf_zig.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/MemoKeyType.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::adapter {
  GRADIDOBLOCKCHAIN_EXPORT MemoKeyType fromGrdw(grdw_memo_key_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdw_memo_key_type toGrdw(MemoKeyType type);
  
  GRADIDOBLOCKCHAIN_EXPORT CrossGroupType fromGrdw(grdw_transaction_body_cross_group_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdw_transaction_body_cross_group_type toGrdw(CrossGroupType type);
  
  GRADIDOBLOCKCHAIN_EXPORT TransactionType fromGrdw(grdw_transaction_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdw_transaction_type toGrdw(TransactionType type);
  
  GRADIDOBLOCKCHAIN_EXPORT AddressType fromGrdw(grdw_address_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdw_address_type toGrdw(AddressType type);
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H