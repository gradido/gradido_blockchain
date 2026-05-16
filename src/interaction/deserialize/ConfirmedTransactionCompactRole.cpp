#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionCompactRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::shared_ptr, std::make_shared;

constexpr size_t STATIC_BUFFER_SIZE = 2048;

namespace gradido {
  using data::compact::ConfirmedGradidoTx;
  namespace interaction::deserialize {

    ConfirmedTransactionCompactRole::ConfirmedTransactionCompactRole(ConstBlockPtr txRaw)
      : mTxRaw(txRaw)
    {
      
    }

    void ConfirmedTransactionCompactRole::run(uint32_t communityIdIndex)
    {
      assert(mTxRaw);      
      GrduStaticBuffer<STATIC_BUFFER_SIZE> buffer;
      buffer.use(
        [&](grd_memory* alloc) -> grd_result
        {
          {
            grdw_confirmed_transaction tx{};
            grd_memory_block src = { .data = (uint8_t*)mTxRaw->data(), .size = mTxRaw->size() };
            auto result = grdw_confirmed_transaction_decode(&tx, &src, alloc);
            // we skip GRD_ERROR_STATIC_BUFFER_TO_SMALL because GrduStaticBuffer should handle this error
            if (GRD_SUCCESS != result && GRD_ERROR_STATIC_BUFFER_TO_SMALL != result) {
              LOG_F(ERROR, "decode error: %s", enum_name(result).data());
              throw GradidoNodeInvalidDataException("error deserialize confirmed transaction");
            }
            mTx = make_shared<ConfirmedGradidoTx>(std::move(ConfirmedGradidoTx::fromGrdw(&tx, communityIdIndex, *g_appContext)));
          }
          alloc->last_index = 0;
          {
            grdw_transaction_body body{};
            grd_memory_block src = { 
              .data = mTx->coldData->bodyBytes.data(), 
              .size = mTx->coldData->bodyBytes.size()
            };
            auto result = grdw_transaction_body_decode(&body, &src, alloc);
            // we skip GRD_ERROR_STATIC_BUFFER_TO_SMALL because GrduStaticBuffer should handle this error
            if (GRD_SUCCESS != result && GRD_ERROR_STATIC_BUFFER_TO_SMALL != result) {
              LOG_F(ERROR, "body decode error: %s", enum_name(result).data());
              throw GradidoNodeInvalidDataException("error deserialize transaction body");
            }
            mTx->fillFromGrdwTransactionBody(&body, *g_appContext);
          }
          return GRD_SUCCESS;
        }
      );      
    }
  }
}
