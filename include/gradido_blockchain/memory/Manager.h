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

	protected:
		Manager();
		// bool mInitalized;
		//std::unordered_map<size_t, BlockStack*> mBlockStacks;
		std::array<BlockStack, kNumBuckets> mBlockStacks;

	};
}

#endif //__GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H