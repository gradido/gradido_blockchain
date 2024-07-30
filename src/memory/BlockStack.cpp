#include "gradido_blockchain/memory/BlockStack.h"
#include "loguru/loguru.hpp"

#include <cstring>
#include <cstdlib>

namespace memory {
	BlockStack::BlockStack(size_t size)
		: mSize(size)
	{
	}
	
	BlockStack::~BlockStack()
	{
		LOG_F(INFO, "release memory page stack: %d, stack size: %d", static_cast<int>(mSize), static_cast<int>(mBlockStack.size()));
		clear();
	}

	uint8_t* BlockStack::getBlock()
	{
		std::scoped_lock _lock(mMutex);

		if (!mSize) {
			return nullptr;
		}
		uint8_t* block = nullptr;
		if (mBlockStack.size() == 0) {
			block = static_cast<uint8_t*>(malloc(mSize));
		} else {
			block = mBlockStack.top();
			mBlockStack.pop();
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

	void BlockStack::clear()
	{
		std::scoped_lock _lock(mMutex);
		while (mBlockStack.size() > 0) {
			auto block = mBlockStack.top();
			mBlockStack.pop();
			free(block);
		}
	}
}
