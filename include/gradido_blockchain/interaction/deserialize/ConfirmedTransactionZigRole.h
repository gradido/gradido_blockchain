#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ZIG_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ZIG_ROLE_H

#include <memory>

namespace memory {
  class Block;
  using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
  namespace data {
    class ConfirmedTransaction;
  }
  namespace interaction::deserialize {
    class ConfirmedTransactionZigRole {
    public:
      ConfirmedTransactionZigRole(memory::ConstBlockPtr bodyBytes);
      ~ConfirmedTransactionZigRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::ConfirmedTransaction> getTransaction() { return mTx; }

    protected:
      memory::ConstBlockPtr mTxRaw;
      std::shared_ptr<const data::ConfirmedTransaction> mTx;
    };
  }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ZIG_ROLE_H