#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H

#include "Block.h"
#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain_core/result.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/types.h"

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
      grd_memory_init_arena_static(&mMemory, mBuffer, BufferSize);
    }

    // memory is reset after call to func
    template<typename Func>
    inline grd_result use(Func&& func)
    {
      grd_result result = func(&mMemory);

      if (mMemory.out_of_memory_capacity || GRD_ERROR_STATIC_BUFFER_TO_SMALL == result || GRD_ERROR_OUT_OF_MEMORY == result) {
        memory::Block dynBuffer((mMemory.capacity + mMemory.out_of_memory_capacity) * 2);
        grd_memory_init_arena_static(&mMemory, dynBuffer.data(), dynBuffer.size());

        result = func(&mMemory);

        if (mMemory.out_of_memory_capacity || GRD_ERROR_STATIC_BUFFER_TO_SMALL == result || GRD_ERROR_OUT_OF_MEMORY == result) {
          LOG_F(
            ERROR,
            "GrduStaticBuffer: out of memory capacity, after retry with: %lu, need at least %lu more bytes",
            dynBuffer.size(),
            mMemory.out_of_memory_capacity
          );
          grd_memory_init_arena_static(&mMemory, mBuffer, BufferSize);
          throw GradidoNodeInvalidDataException("GrduStaticBuffer: out of memory capacity");
        }
      }
      grd_memory_init_arena_static(&mMemory, mBuffer, BufferSize);
      return result;
    }

  protected:
    uint8_t mBuffer[BufferSize];
    grd_memory mMemory;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H