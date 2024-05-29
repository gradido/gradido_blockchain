#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_PROTOBUF_ARENA_BLOCK_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_PROTOBUF_ARENA_BLOCK_H

#include "gradido_blockchain/export.h"

#include <google/protobuf/arena.h>

#include <mutex>
#include <stack>
#include <set>

namespace memory {

	class GRADIDOBLOCKCHAIN_EXPORT ProtobufArenaBlock
	{
	public:
		~ProtobufArenaBlock();
		inline operator google::protobuf::Arena* () { return mArena; }
		static std::shared_ptr<ProtobufArenaBlock> create();
		inline uint64_t getUsedSpace() const { return mArena->SpaceUsed(); }
		inline uint64_t getAllocatedSpace() const { return mArena->SpaceAllocated(); }
	protected:
		ProtobufArenaBlock();
		static void clear();

		google::protobuf::Arena* mArena;

		static std::mutex mProtobufArenaMutex;
		static std::stack<google::protobuf::Arena*> mProtobufArenaStack;
		static std::set<google::protobuf::Arena*> mActiveProtobufArenas;
	};
}

typedef memory::ProtobufArenaBlock ProtobufArenaMemory;

#endif //__GRADIDO_BLOCKCHAIN_MEMORY_PROTOBUF_ARENA_BLOCK_H