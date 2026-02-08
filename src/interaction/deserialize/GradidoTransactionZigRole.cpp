#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionZigRole.h"
#include "gradido_blockchain/memory/Block.h"

#include "gradido_protobuf_zig.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <cassert>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

constexpr size_t STATIC_BUFFER_SIZE = 1024;

namespace gradido {
  using data::GradidoTransaction;
  namespace interaction::deserialize {

    GradidoTransactionZigRole::GradidoTransactionZigRole(ConstBlockPtr txRaw)
      : mTxRaw(txRaw)
    {

    }

    void GradidoTransactionZigRole::run(uint32_t communityIdIndex)
    {
      assert(mTxRaw);
      grdw_gradido_transaction tx;

      auto result = grdw_gradido_transaction_decode(&tx, mTxRaw->data(), mTxRaw->size());
      if (GRDW_ENCODING_ERROR_SUCCESS != result.state) {
        LOG_F(ERROR, "decode error: %s", enum_name(result.state).data());
        throw GradidoNodeInvalidDataException("error deserialize confirmed transaction");
      }

      mTx = GradidoTransaction::fromGrdw(&tx, communityIdIndex);
      grdw_gradido_transaction_free_deep(&tx);
    }
  }
}
