#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_STACK_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_STACK_H

#include <cstdint>
#include <stack>
#include <mutex>

#define MEMORY_MANAGER_MEMORY_PAGE_STACK_MAX_COUNT_PER_SIZE_SMALLER_AS_128 256
#define MEMORY_MANAGER_MEMORY_PAGE_STACK_MAX_COUNT_PER_SIZE_SMALLER_AS_512 32
#define MEMORY_MANAGER_MEMORY_PAGE_STACK_MAX_COUNT_PER_SIZE_SMALLER_BIG 8

namespace memory {
	class BlockStack
	{
	public:
		BlockStack(size_t size);

		uint8_t* getBlock();
		void releaseBlock(uint8_t* memory);

	protected:
		std::stack<uint8_t*> mBlockStack;
		std::mutex mMutex;
		const size_t mSize;
	};

}
#endif //__GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_STACK_H