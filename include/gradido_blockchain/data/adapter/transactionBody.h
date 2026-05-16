#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TRANSACTION_BODY_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TRANSACTION_BODY_H

#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/export.h"
#include "ReturnType.h"

struct grdw_transaction_body;
struct grd_memory;

namespace gradido::data {
  class TransactionBody;
  namespace adapter {
    GRADIDOBLOCKCHAIN_EXPORT TransferAmount fromGrdw(const grdw_transfer_amount& grdwAmount, uint32_t communityIdIndex);
    GRADIDOBLOCKCHAIN_EXPORT grdw_transfer_amount toGrdw(grd_memory* alloc, const TransferAmount& amount, uint32_t communityIdIndex);
  }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TRANSACTION_BODY_H