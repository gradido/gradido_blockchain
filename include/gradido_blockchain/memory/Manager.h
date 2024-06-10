#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H

#include "BlockStack.h"

#include <map>

namespace memory {
	class Manager 
	{
	public:
		static Manager& getInstance();

		uint8_t* getBlock(size_t size);
		void releaseBlock(size_t size, uint8_t* data);

	protected:
		Manager(): mInitalized(true) {}
		~Manager() { mInitalized = false; }

		bool mInitalized;
		std::mutex mBlockStacksMutex;
		std::map<size_t, BlockStack*> mBlockStacks;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H