#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_STATECHANGE_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_STATECHANGE_H

#include "gradido_blockchain/data/Timestamp.h"

namespace gradido::blockchain {
  template<typename T>
  class StateChange {
  public:
    StateChange(uint64_t txId, T state) 
      : mTxId(txId), mState(state) {}
    StateChange(T state)
      : mTxId(0), mState(state) {}

    T getValue() const { return mState; }
    uint64_t getTxId() const { return mTxId; }
    void setTxId(uint64_t txId) { mTxId = txId; }
  private:
    uint64_t mTxId;
    T mState;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_STATECHANGE_H