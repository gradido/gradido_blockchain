#ifndef GRADIDO_BLOCKCHAIN_DATA_ADAPTER_MEMORYBLOCK_H
#define GRADIDO_BLOCKCHAIN_DATA_ADAPTER_MEMORYBLOCK_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_protobuf_zig.h"

#include <memory>

namespace gradido::data::adapter {
  inline memory::ConstBlockPtr fromGrdw(const uint8_t publicKey[32]) {
    return std::make_shared<memory::Block>(32, publicKey);
  }
  // grdwBlock must have enough space to hold the block data
  inline void toGrdw(memory::ConstBlockPtr block, uint8_t* grdwBlock) {
    memcpy(grdwBlock, block->data(), block->size());
  }
}
#endif // GRADIDO_BLOCKCHAIN_DATA_ADAPTER_MEMORYBLOCK_H