#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::shared_ptr;

constexpr size_t STATIC_BUFFER_SIZE = 2048;

namespace gradido {
  using data::ConfirmedTransaction;
  namespace interaction::deserialize {

    ConfirmedTransactionRole::ConfirmedTransactionRole(ConstBlockPtr txRaw)
      : mTxRaw(txRaw)
    {
      
    }

    void ConfirmedTransactionRole::run(uint32_t communityIdIndex)
    {
      assert(mTxRaw);      
      GrduStaticBuffer<STATIC_BUFFER_SIZE> buffer;
      buffer.use(
        [&](grd_memory* alloc) -> grd_result 
        {
          grdw_confirmed_transaction tx{};
          grd_memory_block src = { .data = (uint8_t*)mTxRaw->data(), .size = mTxRaw->size() };
          auto result = grdw_confirmed_transaction_decode(&tx, &src, alloc);
          // we skip GRD_ERROR_STATIC_BUFFER_TO_SMALL because GrduStaticBuffer should handle this error
          if (GRD_SUCCESS != result && GRD_ERROR_STATIC_BUFFER_TO_SMALL != result && GRD_ERROR_OUT_OF_MEMORY != result) {
            LOG_F(ERROR, "decode error: %s", enum_name(result).data());
            throw GradidoNodeInvalidDataException("error deserialize confirmed transaction");
          }
          if (GRD_SUCCESS != result) { return result; }
          // copy data
          mTx = ConfirmedTransaction::fromGrdw(&tx, communityIdIndex);
          return GRD_SUCCESS;
        }
      );      
    }
  }
}
