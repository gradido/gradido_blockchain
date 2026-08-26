#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

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
    class ConfirmedTransactionRole {
    public:
      ConfirmedTransactionRole(memory::ConstBlockPtr bodyBytes);
      ~ConfirmedTransactionRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::ConfirmedTransaction> getTransaction() { return mTx; }

    protected:
      memory::ConstBlockPtr mTxRaw;
      std::shared_ptr<const data::ConfirmedTransaction> mTx;
    };
  }
}

#endif // GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H