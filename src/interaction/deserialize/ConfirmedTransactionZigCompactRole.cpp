#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionZigCompactRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"
#include "gradido_protobuf_zig.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::shared_ptr, std::make_shared;

constexpr size_t STATIC_BUFFER_SIZE = 1024;

namespace gradido {
  using data::compact::ConfirmedGradidoTx;
  namespace interaction::deserialize {

    ConfirmedTransactionZigCompactRole::ConfirmedTransactionZigCompactRole(ConstBlockPtr txRaw)
      : mTxRaw(txRaw)
    {
      
    }

    void ConfirmedTransactionZigCompactRole::run(uint32_t communityIdIndex)
    {
      assert(mTxRaw);      
      GrduStaticBuffer<STATIC_BUFFER_SIZE> buffer;
      buffer.use(
        [&](grdu_memory* alloc) -> int
        {
          {
            grdw_confirmed_transaction tx{};
            auto result = grdw_confirmed_transaction_decode(alloc, &tx, mTxRaw->data(), mTxRaw->size());
            // we skip GRDW_ENCODING_ERROR_C_ALLOC_FAILED because GrduStaticBuffer should handle this error
            if (GRDW_ENCODING_ERROR_SUCCESS != result.state && GRDW_ENCODING_ERROR_C_ALLOC_FAILED != result.state) {
              LOG_F(ERROR, "decode error: %s", enum_name(result.state).data());
              throw GradidoNodeInvalidDataException("error deserialize confirmed transaction");
            }
            mTx = make_shared<ConfirmedGradidoTx>(std::move(ConfirmedGradidoTx::fromGrdw(&tx, communityIdIndex, *g_appContext)));
          }
          alloc->last_index = 0;
          {
            grdw_transaction_body body{};
            auto result = grdw_transaction_body_decode(alloc, &body, mTx->coldData->bodyBytes.data(), mTx->coldData->bodyBytes.size());
            // we skip GRDW_ENCODING_ERROR_C_ALLOC_FAILED because GrduStaticBuffer should handle this error
            if (GRDW_ENCODING_ERROR_SUCCESS != result.state && GRDW_ENCODING_ERROR_C_ALLOC_FAILED != result.state) {
              LOG_F(ERROR, "body decode error: %s", enum_name(result.state).data());
              throw GradidoNodeInvalidDataException("error deserialize transaction body");
            }
            mTx->fillFromGrdwTransactionBody(&body, *g_appContext);
          }
          return 0;
        }
      );      
    }
  }
}
