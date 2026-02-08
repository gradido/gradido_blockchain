#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyZigRole.h"
#include "gradido_blockchain/memory/Block.h"

#include "gradido_protobuf_zig.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;
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

      auto result = grdw_transaction_body_decode(&body, mBodyBytes->data(), mBodyBytes->size());      
      if (GRDW_ENCODING_ERROR_SUCCESS != result.state) {
        LOG_F(ERROR, "decode error: %s", enum_name(result.state).data());
        throw GradidoNodeInvalidDataException("error deserialize body bytes");
      }

      mTransactionBody = TransactionBody::fromGrdwTransactionBody(&body, communityIdIndex);
      grdw_transaction_body_free_deep(&body);
    }
  }
}
