#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_STATECHANGE_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_STATECHANGE_H

#include "gradido_blockchain/data/Timestamp.h"

namespace gradido::blockchain {
  template<typename T>
  class StateChange
  {
  public:
    StateChange(uint64_t txId, T state, bool existInIndex = true)
      : mTxId(txId), mState(state), mExistInIndex(existInIndex) {}
    StateChange(T state, bool existInIndex = true)
      : mTxId(0), mState(state), mExistInIndex(existInIndex) {}

    T getValue() const { return mState; }
    uint64_t getTxId() const { return mTxId; }
    void setTxId(uint64_t txId) { mTxId = txId; }
    inline  bool ífExistInIndex() const { return mExistInIndex; }
  protected:
    uint64_t mTxId;
    T mState;
    // tell if state was found in index
    bool mExistInIndex;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_STATECHANGE_H