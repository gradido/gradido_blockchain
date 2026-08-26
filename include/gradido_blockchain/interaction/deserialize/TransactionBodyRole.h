#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H

#include <memory>

namespace memory {
  class Block;
  using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
  namespace data {
    class TransactionBody;
  }
  namespace interaction::deserialize {
    class TransactionBodyRole {
    public:
      TransactionBodyRole(memory::ConstBlockPtr bodyBytes);
      ~TransactionBodyRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::TransactionBody> getBody() { return mTransactionBody; }

    protected:
      memory::ConstBlockPtr mBodyBytes;
      std::shared_ptr<const data::TransactionBody> mTransactionBody;
    };
  }
}

#endif // GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H