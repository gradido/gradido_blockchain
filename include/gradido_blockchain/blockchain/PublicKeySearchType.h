#ifndef GRADIDO_BLOCKCHAIN_PUBLIC_KEY_SEARCH_TYPE_H
#define GRADIDO_BLOCKCHAIN_PUBLIC_KEY_SEARCH_TYPE_H

#include "gradido_blockchain/types.h"

namespace gradido::blockchain {
  enum PublicKeySearchType : uint8_t 
  {
    None,
    InvolvedPublicKey,
    BalanceChangingPublicKey,
    MissingIndex
  };
}
#endif // GRADIDO_BLOCKCHAIN_PUBLIC_KEY_SEARCH_TYPE_H