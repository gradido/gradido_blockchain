#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_LEDGER_ANCHOR_H__
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_LEDGER_ANCHOR_H__

#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/export.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data {  
  namespace adapter {
    GRADIDOBLOCKCHAIN_EXPORT LedgerAnchor fromGrdw(const grdw_ledger_anchor& ledgerAnchor);
    GRADIDOBLOCKCHAIN_EXPORT grdw_ledger_anchor toGrdw(grdu_memory* alloc, const LedgerAnchor& ledgerAnchor);
  }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_LEDGER_ANCHOR_H__