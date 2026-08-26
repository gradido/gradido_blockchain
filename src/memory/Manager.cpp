#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Manager.h"
#include <cassert>
#include <cstring>

namespace memory {
	Manager::Manager() : mBlockStacks({
			{ 16 },
			{ 32 },
			{ 64 },
			{ 128 },
			{ 256 },
			{ 512 },
			{ 1024 },
			{ 2048 }
		})
	{
		;
		// mInitalized = true;
	}
	Manager::~Manager()
	{
		//mBlockStacks.clear();
		// mInitalized = false;
	}
	Manager* Manager::getInstance()
	{
		static Manager one;
		return &one;
	}

	uint8_t* Manager::getBlock(size_t size)
	{
		if (0 == size) {
			return nullptr;
		}
		auto bucketIndex = roundUpToBucket(size);

		if (bucketIndex < 0) {
			auto data = static_cast<uint8_t*>(malloc(size));
			if (!data) {
				throw GradidoMemoryAllocationFailed("BlockStack Allocation failed", size);
			}
			memset(data, 0, size);
			return data;
		}
		return mBlockStacks[bucketIndex].getBlock();
	}

	void Manager::releaseBlock(size_t size, uint8_t* data)
	{
		if (0 == size) {
			return;
		}

		auto bucketIndex = roundUpToBucket(size);
		if (bucketIndex < 0) {
			free(data);
		}
		else {
			mBlockStacks[bucketIndex].releaseBlock(data);
		}
	}

}