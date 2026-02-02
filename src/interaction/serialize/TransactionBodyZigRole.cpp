#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/serialize/TransactionBodyZigRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_protobuf_zig.h"

#include <memory>

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

namespace gradido::interaction::serialize {
    ConstBlockPtr TransactionBodyZigRole::run() const 
    { 
      static thread_local uint8_t staticResultBuffer[1024];
      memset(staticResultBuffer, 0, 1024);
      grdw_transaction_body body;
      mBody.toGrdw(&body);
      auto encodeResult = grdw_transaction_body_encode(&body, staticResultBuffer, 1024);
      grdw_transaction_body_free_deep(&body);
      size_t bodyBytesSize = encodeResult;
      while (!staticResultBuffer[--bodyBytesSize]);
      return make_shared<const Block>(bodyBytesSize, staticResultBuffer);
    }
    
    size_t TransactionBodyZigRole::calculateSerializedSize() const 
    {
      return 0;
    }
}
