#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::shared_ptr;

constexpr size_t STATIC_BUFFER_SIZE = 1024;

namespace gradido {
  using data::TransactionBody;
  namespace interaction::deserialize {

    TransactionBodyRole::TransactionBodyRole(ConstBlockPtr bodyBytes)
      : mBodyBytes(bodyBytes)
    {
      
    }

    void TransactionBodyRole::run(uint32_t communityIdIndex)
    {
      assert(mBodyBytes);
      GrduStaticBuffer<STATIC_BUFFER_SIZE> buffer;

      buffer.use(
        [&](grd_memory* alloc) -> grd_result 
        {
          grdw_transaction_body body{};
          grd_memory_block src = { .data = (uint8_t*)mBodyBytes->data(), .size = mBodyBytes->size() };
          auto result = grdw_transaction_body_decode(&body, &src, alloc);
          // we skip GRD_ERROR_STATIC_BUFFER_TO_SMALL because GrduStaticBuffer should handle this error
          if (GRD_SUCCESS != result && GRD_ERROR_STATIC_BUFFER_TO_SMALL != result && GRD_ERROR_OUT_OF_MEMORY != result) {
            LOG_F(ERROR, "decode error: %s", enum_name(result).data());
            throw GradidoNodeInvalidDataException("error deserialize body bytes");
          }
          if (GRD_SUCCESS != result) { return result; }
          mTransactionBody = TransactionBody::fromGrdw(&body, communityIdIndex);
          return GRD_SUCCESS;
        }
      );
    }
  }
}
