#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_COMPACT_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_COMPACT_ROLE_H

#include <memory>

namespace memory {
  class Block;
  using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
  namespace data::compact {
    struct ConfirmedGradidoTx;
  }
  namespace interaction::deserialize {
    class ConfirmedTransactionCompactRole {
    public:
      ConfirmedTransactionCompactRole(memory::ConstBlockPtr bodyBytes);
      ~ConfirmedTransactionCompactRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::compact::ConfirmedGradidoTx> getTransaction() { return mTx; }

    protected:
      memory::ConstBlockPtr mTxRaw;
      std::shared_ptr<data::compact::ConfirmedGradidoTx> mTx;
    };
  }
}

#endif // GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_COMPACT_ROLE_H