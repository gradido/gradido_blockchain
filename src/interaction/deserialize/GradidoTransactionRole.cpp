#include "gradido_blockchain_core/data/wire/gradido_transaction.h"
#include "gradido_blockchain_core/result.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
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
  using data::GradidoTransaction;
  namespace interaction::deserialize {

    GradidoTransactionRole::GradidoTransactionRole(ConstBlockPtr txRaw)
      : mTxRaw(txRaw)
    {

    }

    void GradidoTransactionRole::run(uint32_t communityIdIndex)
    {
      assert(mTxRaw);
      
      GrduStaticBuffer<STATIC_BUFFER_SIZE> buffer;
      buffer.use(
        [&](arnm* alloc) -> arnm_result
        {
          grdw_gradido_transaction tx{};
          arnm_memory_block src = { .data = (uint8_t*)mTxRaw->data(), .size = static_cast<uint32_t>(mTxRaw->size()) };
          auto result = grdw_gradido_transaction_decode(&tx, &src, alloc);
          // we skip ARNM_ERROR_OUT_OF_MEMORY because GrduStaticBuffer should handle this error
          if (ARNM_SUCCESS != result && ARNM_ERROR_OUT_OF_MEMORY != result) {
            LOG_F(ERROR, "decode error: %s", grd_result_to_string(result));
            throw GradidoNodeInvalidDataException("error deserialize transaction");
          }
          if (ARNM_SUCCESS != result) { return result; }
          mTx = GradidoTransaction::fromGrdw(&tx, communityIdIndex);
          return ARNM_SUCCESS;
        }
      );
    }
  }
}
