#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_ARRAY_BUFFER_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_ARRAY_BUFFER_H

#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

namespace gradido::data::adapter {
  template<std::size_t N>
  inline memory::ConstBlockPtr toConstBlockPtr(const ByteArray<N>& array)
  {
    return std::make_shared<const memory::Block>(N, array.data());
  }

  template<std::size_t N>
  inline ByteArray<N> toByteArray(const memory::Block& block)
  {
    if (block.size() != N) {
      throw GradidoNodeInvalidDataException("toByteArray called with wrong sized block");
    }
    return ByteArray<N>(block.data());
  }

  template<std::size_t N>
  inline ByteArray<N> toByteArray(memory::ConstBlockPtr block)
  {
    return toByteArray<N>(*block);
  }
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_ADAPTER_ARRAY_BUFFER_H