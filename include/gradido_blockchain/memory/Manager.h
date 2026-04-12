#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H

#include "BlockStack.h"
#include "gradido_blockchain/export.h"

#include <array>
constexpr size_t kBuckets[] = { 16,32,64,128,256,512,1024,2048 };
constexpr size_t kNumBuckets = sizeof(kBuckets) / sizeof(kBuckets[0]);

namespace memory {
	class GRADIDOBLOCKCHAIN_EXPORT Manager
	{
	public:
		static Manager* getInstance();
		~Manager();

		uint8_t* getBlock(size_t size);
		void releaseBlock(size_t size, uint8_t* data);

		inline static int roundUpToBucket(size_t size);

	protected:
		Manager();
		// bool mInitalized;
		//std::unordered_map<size_t, BlockStack*> mBlockStacks;
		 std::array<BlockStack, kNumBuckets> mBlockStacks;
	};

	int Manager::roundUpToBucket(size_t size)
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
}

#endif //__GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H