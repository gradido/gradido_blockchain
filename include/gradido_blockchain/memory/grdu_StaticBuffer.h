#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H

#include "Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/types.h"
#include "gradido_protobuf_zig.h"

#include "loguru/loguru.hpp"

#include <functional>

// helper class for using grdu_memory from C-Interface 
// init with static buffer and try, if not enough space, try again with dynamic allocator
namespace memory {
  template<size_t BufferSize>
  class GrduStaticBuffer 
  {
  public:
    GrduStaticBuffer() {
      grdu_memory_init_static(&mMemory, mBuffer, BufferSize);
    }

    // memory is reset after call to func
    template<typename Func>
    inline auto use(Func&& func)
    {
      auto result = func(&mMemory);

      if (mMemory.out_of_memory_capacity) {
        memory::Block dynBuffer(mMemory.capacity + mMemory.out_of_memory_capacity + 128);
        grdu_memory_init_static(&mMemory, dynBuffer.data(), dynBuffer.size());

        result = func(&mMemory);

        if (mMemory.out_of_memory_capacity) {
          LOG_F(
            ERROR, 
            "GrduStaticBuffer: out of memory capacity, after retry with: %llu, need at least %llu more bytes",
            dynBuffer.size(),
            mMemory.out_of_memory_capacity
          );
          grdu_memory_init_static(&mMemory, mBuffer, BufferSize);
          throw GradidoNodeInvalidDataException("GrduStaticBuffer: out of memory capacity");
        }
      }
      grdu_memory_init_static(&mMemory, mBuffer, BufferSize);
      return result;
    }
    
  protected:
    uint8_t mBuffer[BufferSize];
    grdu_memory mMemory;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H