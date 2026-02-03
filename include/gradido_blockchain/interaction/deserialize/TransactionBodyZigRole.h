#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTIONBODYZIGROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTIONBODYZIGROLE_H

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
    class TransactionBodyZigRole {
    public:
      TransactionBodyZigRole(memory::ConstBlockPtr bodyBytes);
      ~TransactionBodyZigRole() = default;

      void run(uint32_t communityIdIndex);
      inline std::shared_ptr<const data::TransactionBody> getBody() { return mTransactionBody; }

    protected:
      memory::ConstBlockPtr mBodyBytes;
      std::shared_ptr<const data::TransactionBody> mTransactionBody;
    };
  }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTIONBODYZIGROLE_H