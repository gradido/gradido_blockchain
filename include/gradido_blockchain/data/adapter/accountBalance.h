#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_ACCOUNT_BALANCE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_ACCOUNT_BALANCE_H

#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/export.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data {
  namespace adapter {
    GRADIDOBLOCKCHAIN_EXPORT AccountBalance fromGrdw(const grdw_account_balance& grdwAccountBalance, uint32_t communityIdIndex);
    GRADIDOBLOCKCHAIN_EXPORT grdw_account_balance toGrdw(const AccountBalance& grdwAccountBalance, uint32_t communityIdIndex);
  }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_ACCOUNT_BALANCE_H