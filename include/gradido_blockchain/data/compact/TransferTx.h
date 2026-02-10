#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__

#include "PublicKeyIndex.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_protobuf_zig.h"

#include <set>

namespace gradido::data::compact {
  struct GRADIDOBLOCKCHAIN_EXPORT TransferTx 
  {
    int64_t amountGddCent; // 8 Bytes
    PublicKeyIndex senderPublicKeyIndex; // 8 Bytes
    PublicKeyIndex recipientPublicKeyIndex; // 8 Bytes

    inline static TransferTx fromGrdw(const grdw_gradido_transfer* tx_transfer, uint32_t senderCommunityIdIndex, uint32_t recipientCommunityIdIndex)
    {
      return {
        .amountGddCent = tx_transfer->sender.amount,
        .senderPublicKeyIndex = PublicKeyIndex::fromPublicKey(senderCommunityIdIndex, tx_transfer->sender.pubkey),
        .recipientPublicKeyIndex = PublicKeyIndex::fromPublicKey(recipientCommunityIdIndex, tx_transfer->recipient)
      };
    }

    inline bool isInvolved(PublicKeyIndex other) const {
      return 
        other.publicKeyIndex == senderPublicKeyIndex.publicKeyIndex || 
        other.publicKeyIndex == recipientPublicKeyIndex.publicKeyIndex;
    }
    inline void getInvolvedAddresses(std::set<PublicKeyIndex>& involvedAddresses) const {
      involvedAddresses.insert(senderPublicKeyIndex);
      involvedAddresses.insert(recipientPublicKeyIndex);
    }
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_TRANSFER_TX_H__