#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionZigRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"
#include "gradido_protobuf_zig.h"

#include "gradido_protobuf_zig.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::shared_ptr;

constexpr size_t STATIC_BUFFER_SIZE = 1024;

namespace gradido {
  using data::ConfirmedTransaction;
  namespace interaction::deserialize {

    ConfirmedTransactionZigRole::ConfirmedTransactionZigRole(ConstBlockPtr txRaw)
      : mTxRaw(txRaw)
    {
      
    }

    void ConfirmedTransactionZigRole::run(uint32_t communityIdIndex)
    {
      assert(mTxRaw);      
      GrduStaticBuffer<STATIC_BUFFER_SIZE> buffer;
      mTx = buffer.use(
        [&](grdu_memory* alloc) -> shared_ptr<const ConfirmedTransaction> 
        {
          grdw_confirmed_transaction tx{};
          auto result = grdw_confirmed_transaction_decode(alloc, &tx, mTxRaw->data(), mTxRaw->size());
          // we skip GRDW_ENCODING_ERROR_C_ALLOC_FAILED because GrduStaticBuffer should handle this error
          if (GRDW_ENCODING_ERROR_SUCCESS != result.state && GRDW_ENCODING_ERROR_C_ALLOC_FAILED != result.state) {
            LOG_F(ERROR, "decode error: %s", enum_name(result.state).data());
            throw GradidoNodeInvalidDataException("error deserialize confirmed transaction");
          }
          // copy data
          return ConfirmedTransaction::fromGrdw(&tx, communityIdIndex);
        }
      );      
    }
  }
}
