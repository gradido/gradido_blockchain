#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ZIG_COMPACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ZIG_COMPACT_ROLE_H

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
    class ConfirmedTransactionZigCompactRole {
    public:
      ConfirmedTransactionZigCompactRole(memory::ConstBlockPtr bodyBytes);
      ~ConfirmedTransactionZigCompactRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::compact::ConfirmedGradidoTx> getTransaction() { return mTx; }

    protected:
      memory::ConstBlockPtr mTxRaw;
      std::shared_ptr<data::compact::ConfirmedGradidoTx> mTx;
    };
  }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ZIG_COMPACT_ROLE_H