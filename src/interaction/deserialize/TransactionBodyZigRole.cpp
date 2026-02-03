#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyZigRole.h"
#include "gradido_blockchain/memory/Block.h"

#include "gradido_protobuf_zig.h"

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

constexpr size_t STATIC_BUFFER_SIZE = 1024;

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
      if (mBodyBytes->size() >= STATIC_BUFFER_SIZE - 16) {
        throw GradidoNodeInvalidDataException("Input body Bytes larger then static buffer - 16");
      }
      static thread_local uint8_t staticBuffer[STATIC_BUFFER_SIZE];
      memset(staticBuffer, 0, STATIC_BUFFER_SIZE);
      memcpy(staticBuffer, mBodyBytes->data(), mBodyBytes->size());
      int result = 0;
      for (int i = 0; i < 16; ++i) {
        result = grdw_transaction_body_decode(&body, staticBuffer, mBodyBytes->size()+i);
        if (result > 0) break;
      }
      if (result <= 0) {
        throw GradidoNodeInvalidDataException("error deserialize body bytes, after adding increasingly more zero until 16 where added");
      }
      mTransactionBody = TransactionBody::fromGrdwTransactionBody(&body, communityIdIndex);
      grdw_transaction_body_free_deep(&body);
    }
  }
}
