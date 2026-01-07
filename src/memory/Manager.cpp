#include "gradido_blockchain/memory/Manager.h"
#include <cassert>
#include <cstring>

inline static int roundUpToBucket(size_t size)
{
	if (size <= 16)  return 0;
	if (size <= 32)  return 1;
	if (size <= 64)  return 2;
	if (size <= 128) return 3;
	if (size <= 256) return 4;
	if (size <= 512) return 5;
	if (size <= 1024)return 6;
	if (size <= 2048)return 7;
	return -1;
}

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