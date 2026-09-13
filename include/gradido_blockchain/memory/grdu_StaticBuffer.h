#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H

#include "Block.h"
#include "arnm/arena.h"
#include "arnm/memory_block.h"
#include "gradido_blockchain_core/result.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/types.h"

#include "loguru/loguru.hpp"

#include <functional>

// helper class for using arnm from C-Interface
// init with static buffer and try, if not enough space, try again with dynamic allocator
namespace memory {
  template<size_t BufferSize>
  class GrduStaticBuffer
  {
    // arnm_init_arena_borrow refuses a capacity which isn't a multiple of 8, it would else
    // let the arena index walk past the end of a buffer sized exactly
    static_assert(BufferSize > 0 && BufferSize % 8 == 0, "BufferSize must be a positive multiple of 8");
    static_assert(BufferSize <= ARNM_MAX_ALLOC_SIZE, "BufferSize must fit into arnm's uint32_t capacity");

  public:
    GrduStaticBuffer() {
      arnm_init_arena_borrow(&mMemory, mBuffer, static_cast<uint32_t>(BufferSize));
    }

    // memory is reset after call to func
    template<typename Func>
    inline arnm_result use(Func&& func)
    {
      arnm_result result = func(&mMemory);

      auto overflow = arnm_arena_overflow_total(&mMemory);
      if (overflow || ARNM_ERROR_OUT_OF_MEMORY == result) {
        // the allocator is opaque since arnm 0.6.0, so the capacity is the one we gave it
        size_t dynSize = (BufferSize + overflow) * 2;
        // keep the multiple of 8 arnm_init_arena_borrow asks for
        dynSize = (dynSize + 7) & ~static_cast<size_t>(7);
        if (dynSize > ARNM_MAX_ALLOC_SIZE) {
          throw GradidoNodeInvalidDataException("GrduStaticBuffer: needed capacity exceeds arnm max alloc size");
        }
        memory::Block dynBuffer(dynSize);
        arnm_init_arena_borrow(&mMemory, dynBuffer.data(), static_cast<uint32_t>(dynSize));

        result = func(&mMemory);

        overflow = arnm_arena_overflow_total(&mMemory);
        if (overflow || ARNM_ERROR_OUT_OF_MEMORY == result) {
          LOG_F(
            ERROR,
            "GrduStaticBuffer: out of memory capacity, after retry with: %zu, need at least %zu more bytes",
            dynSize,
            overflow
          );
          arnm_init_arena_borrow(&mMemory, mBuffer, static_cast<uint32_t>(BufferSize));
          throw GradidoNodeInvalidDataException("GrduStaticBuffer: out of memory capacity");
        }
      }
      arnm_init_arena_borrow(&mMemory, mBuffer, static_cast<uint32_t>(BufferSize));
      return result;
    }

  protected:
    // arnm_init_arena_borrow promises every block it hands out is 8 byte aligned, so it
    // refuses a base address which isn't
    alignas(8) uint8_t mBuffer[BufferSize];
    arnm mMemory;
  };
}

#endif // __GRADIDO_BLOCKCHAIN_MEMORY_GRDU_STATIC_BUFFER_H
