#include "gradido_blockchain/memory/Manager.h"
#include <cassert>
#include <cstring>

namespace memory {
	Manager::Manager(): mInitalized(true)
	{
	}
	Manager::~Manager()
	{
		mInitalized = false;
	}
	Manager* Manager::getInstance()
	{
		static Manager one;
		return &one;
	}

	uint8_t* Manager::getBlock(size_t size)
	{
		if (!mInitalized) {
			return static_cast<uint8_t*>(malloc(size));
		}
		if (size == 0) return nullptr;
		{
			std::shared_lock _lock(mBlockStacksMutex);
			auto it = mBlockStacks.find(size);
			if (it != mBlockStacks.end()) {
				return it->second->getBlock();
			}
		}
		std::unique_lock _lock(mBlockStacksMutex);
		auto it = mBlockStacks.insert({ size, new BlockStack(size) }).first;
		return it->second->getBlock();
	}

	void Manager::releaseBlock(size_t size, uint8_t* data)
	{
		if (!mInitalized) {
			free(data);
			return;
		}
		std::shared_lock _lock(mBlockStacksMutex);
		auto it = mBlockStacks.find(size);
		if (it != mBlockStacks.end()) {
			it->second->releaseBlock(data);
		}
		else {
			free(data);
		}
	}

}