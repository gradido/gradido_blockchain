#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H

#include "gradido_blockchain_core/types/address.h"
#include "gradido_blockchain_core/types/balance_derivation.h"
#include "gradido_blockchain_core/types/cross_group.h"
#include "gradido_blockchain_core/types/ledger_anchor.h"
#include "gradido_blockchain_core/types/transaction.h"
#include "gradido_blockchain_core/data/wire/ledger_anchor.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/MemoKeyType.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/export.h"

namespace gradido::data::adapter {
  GRADIDOBLOCKCHAIN_EXPORT MemoKeyType fromGrdw(grdt_memo_key type);
  GRADIDOBLOCKCHAIN_EXPORT grdt_memo_key toGrdw(MemoKeyType type);

  GRADIDOBLOCKCHAIN_EXPORT CrossGroupType fromGrdw(grdt_cross_group type);
  GRADIDOBLOCKCHAIN_EXPORT grdt_cross_group toGrdw(CrossGroupType type);

  GRADIDOBLOCKCHAIN_EXPORT TransactionType fromGrdw(grdt_transaction type);
  GRADIDOBLOCKCHAIN_EXPORT grdt_transaction toGrdw(TransactionType type);

  GRADIDOBLOCKCHAIN_EXPORT AddressType fromGrdw(grdt_address type);
  GRADIDOBLOCKCHAIN_EXPORT grdt_address toGrdw(AddressType type);

  GRADIDOBLOCKCHAIN_EXPORT LedgerAnchor::Type fromGrdw(grdt_ledger_anchor type);
  GRADIDOBLOCKCHAIN_EXPORT grdt_ledger_anchor toGrdw(LedgerAnchor::Type type);

  GRADIDOBLOCKCHAIN_EXPORT BalanceDerivationType fromGrdw(grdt_balance_derivation type);
  GRADIDOBLOCKCHAIN_EXPORT grdt_balance_derivation toGrdw(BalanceDerivationType type);

}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TYPES_H