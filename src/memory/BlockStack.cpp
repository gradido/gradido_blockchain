#include "gradido_blockchain/memory/BlockStack.h"

#include <cstring>
namespace memory {
	BlockStack::BlockStack(size_t size)
		: mSize(size)
	{
	}
	
	uint8_t* BlockStack::getBlock()
	{
		if (!mSize) {
			return nullptr;
		}
		uint8_t* block = nullptr;
		{
			std::scoped_lock _lock(mMutex);
			if (!mBlockStack.empty()) {
				block = mBlockStack.top();
				mBlockStack.pop();
			}
		}
		if (!block) {
			block = static_cast<uint8_t*>(malloc(mSize));
		}
		memset(block, 0, mSize);
		return block;
	}

	void BlockStack::releaseBlock(uint8_t* memory)
	{
		if (!memory) return;
		std::scoped_lock _lock(mMutex);

		if (
			(mSize < 128 && mBlockStack.size() > MEMORY_MANAGER_MEMORY_PAGE_STACK_MAX_COUNT_PER_SIZE_SMALLER_AS_128) ||
			(mSize < 512 && mBlockStack.size() > MEMORY_MANAGER_MEMORY_PAGE_STACK_MAX_COUNT_PER_SIZE_SMALLER_AS_512) ||
			(mSize >= 512 && mBlockStack.size() > MEMORY_MANAGER_MEMORY_PAGE_STACK_MAX_COUNT_PER_SIZE_SMALLER_BIG)
			)
		{
			free(memory);
		}
		else {
			mBlockStack.push(memory);
		}
	}
}
