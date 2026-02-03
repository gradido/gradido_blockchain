#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyZigRole.h"
#include "gradido_blockchain/memory/Block.h"

#include "gradido_protobuf_zig.h"

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

namespace gradido {
  using data::TransactionBody;
  namespace interaction::deserialize {

    TransactionBodyZigRole::TransactionBodyZigRole(ConstBlockPtr bodyBytes)
      : mBodyBytes(bodyBytes)
    {

    }

    void TransactionBodyZigRole::run(uint32_t communityIdIndex)
    {
      assert(mBodyBytes);
      grdw_transaction_body body;
      auto result = grdw_transaction_body_decode(&body, mBodyBytes->data(), mBodyBytes->size());
      if (result <= 0) {
        throw GradidoNodeInvalidDataException("error deserialize body bytes");
      }
      mTransactionBody = TransactionBody::fromGrdwTransactionBody(&body, communityIdIndex);
      grdw_transaction_body_free_deep(&body);
    }
  }
}
