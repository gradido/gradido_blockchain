#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSACTION_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSACTION_ROLE_H

#include <memory>

namespace memory {
  class Block;
  using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
  namespace data {
    class GradidoTransaction;
  }
  namespace interaction::deserialize {
    class GradidoTransactionRole {
    public:
      GradidoTransactionRole(memory::ConstBlockPtr bodyBytes);
      ~GradidoTransactionRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::GradidoTransaction> getTransaction() { return mTx; }

    protected:
      memory::ConstBlockPtr mTxRaw;
      std::shared_ptr<const data::GradidoTransaction> mTx;
    };
  }
}

#endif //GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSACTION_ROLE_H