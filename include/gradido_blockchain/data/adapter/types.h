#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H

#include "gradido_blockchain_core/data/address_type.h"
#include "gradido_blockchain_core/data/balance_derivation_type.h"
#include "gradido_blockchain_core/data/cross_group_type.h"
#include "gradido_blockchain_core/data/transaction_type.h"
#include "gradido_blockchain_core/data/wire/ledger_anchor.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/MemoKeyType.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::adapter {
  GRADIDOBLOCKCHAIN_EXPORT MemoKeyType fromGrdw(grdw_memo_key_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdw_memo_key_type toGrdw(MemoKeyType type);

  GRADIDOBLOCKCHAIN_EXPORT CrossGroupType fromGrdw(grdd_cross_group_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdd_cross_group_type toGrdw(CrossGroupType type);

  GRADIDOBLOCKCHAIN_EXPORT TransactionType fromGrdw(grdd_transaction_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdd_transaction_type toGrdw(TransactionType type);

  GRADIDOBLOCKCHAIN_EXPORT AddressType fromGrdw(grdd_address_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdd_address_type toGrdw(AddressType type);

  GRADIDOBLOCKCHAIN_EXPORT LedgerAnchor::Type fromGrdw(grdw_ledger_anchor_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdw_ledger_anchor_type toGrdw(LedgerAnchor::Type type);

  GRADIDOBLOCKCHAIN_EXPORT BalanceDerivationType fromGrdw(grdd_balance_derivation_type type);
  GRADIDOBLOCKCHAIN_EXPORT grdd_balance_derivation_type toGrdw(BalanceDerivationType type);

}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H