#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"

namespace gradido::data::compact {
  RegisterAddressTx RegisterAddressTx::fromGrdw(const grdw_register_address* grdw_register_address, uint32_t blockchainCommunityIdIndex)
  {
    RegisterAddressTx registerAddress;
    registerAddress.addressType = adapter::fromGrdw(grdw_register_address->address_type);
    registerAddress.derivationIndex = grdw_register_address->derivation_index;
    registerAddress.nameHashIndex = g_appContext->getOrAddUserNameHashIndex(grdw_register_address->name_hash);
    registerAddress.userPublicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_register_address->user_pubkey);
    registerAddress.accountPublicKeyIndex = PublicKeyIndex::fromPublicKey(blockchainCommunityIdIndex, grdw_register_address->account_pubkey);
    return registerAddress;
  }
}