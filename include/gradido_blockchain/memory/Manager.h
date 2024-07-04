#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H

#include "BlockStack.h"
#include "gradido_blockchain/export.h"

#include <map>

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

		bool mInitalized;
		std::mutex mBlockStacksMutex;
		std::map<size_t, BlockStack*> mBlockStacks;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MEMORY_MANAGER_H