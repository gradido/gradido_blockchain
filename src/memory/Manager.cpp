#include "gradido_blockchain/memory/Manager.h"
#include <cassert>

namespace memory {
	Manager& Manager::getInstance() 
	{
		static Manager one;
		return one;
	}

	uint8_t* Manager::getBlock(size_t size)
	{
		assert(size > 0);
		if (!mInitalized) return (uint8_t*)malloc(size);
		std::lock_guard _lock(mBlockStacksMutex);
		auto it = mBlockStacks.find(size);
		if (it == mBlockStacks.end()) {
			it = mBlockStacks.insert({ size, new BlockStack(size) }).first;
		}
		return it->second->getBlock();
	}

	void Manager::releaseBlock(size_t size, uint8_t* data)
	{
		if (!mInitalized) {
			free(data);
			return;
		}
		std::lock_guard _lock(mBlockStacksMutex);
		auto it = mBlockStacks.find(size);
		if (it != mBlockStacks.end()) {
			it->second->releaseBlock(data);
		}
		else {
			free(data);
		}
	}

}