#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"

namespace gradido::data {
  namespace compact {
    ConfirmedGradidoTxCold::ConfirmedGradidoTxCold()
      : createdAtSeconds(0), paringTxNr(0), createdAtNanos(0), pairingTxCommunityIdIndex(0), bodyBytes(0)
    {

    }
  }
}