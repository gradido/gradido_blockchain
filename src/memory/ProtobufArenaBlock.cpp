#include "gradido_blockchain/memory/ProtobufArenaBlock.h"
#include "gradido_blockchain/memory/Manager.h"

namespace memory {

	std::mutex ProtobufArenaBlock::mProtobufArenaMutex;
	std::stack<google::protobuf::Arena*> ProtobufArenaBlock::mProtobufArenaStack;
	std::set<google::protobuf::Arena*> ProtobufArenaBlock::mActiveProtobufArenas;

	ProtobufArenaBlock::ProtobufArenaBlock()
		: mArena(nullptr)
	{
		std::scoped_lock _lock(mProtobufArenaMutex);
		google::protobuf::Arena* arena;

		if (mProtobufArenaStack.size()) {
			arena = mProtobufArenaStack.top();
			mProtobufArenaStack.pop();
		}
		else {
			google::protobuf::ArenaOptions options;
			options.start_block_size = 3584;
			arena = new google::protobuf::Arena(options);
			//printf("new arena memory, active: %d\n", mActiveProtobufArenas.size());
		}
		if (!mActiveProtobufArenas.insert(arena).second) {
			assert(false && "[Manager::getProtobufArenaMemory] try to return protobuf memory already in use");
		}

		mArena = arena;
	}

	void ProtobufArenaBlock::clear()
	{
		std::scoped_lock _lock(mProtobufArenaMutex);
		while (mProtobufArenaStack.size() > 0) {
			auto arena = mProtobufArenaStack.top();
			mProtobufArenaStack.pop();
			delete arena;
		}
	}

	ProtobufArenaBlock::~ProtobufArenaBlock()
	{
		if (!mArena) return;
		std::scoped_lock _lock(mProtobufArenaMutex);

		if (!mActiveProtobufArenas.erase(mArena)) {
			assert(false && "[Manager::releaseMemory] try to remove protobuf arena memory already removed");
		}

		if (mArena->SpaceAllocated() > 7168) {
			//printf("delete protobuf arena space allocated: %d, still active: %d\n", memory->SpaceAllocated(), mActiveProtobufArenas.size());
			delete mArena;
		}
		else {
			mProtobufArenaStack.push(mArena);
			auto usedSpaceBefore = mArena->SpaceUsed();
			auto usedSpace = mArena->Reset();
			if (usedSpace > 7168) {
				int zahl = 1;
			}
			//printf("release protobuf arena used size: %d, still active: %d\n", usedSpace, mActiveProtobufArenas.size());
		}
		mArena = nullptr;
	}

	std::shared_ptr<ProtobufArenaBlock> ProtobufArenaBlock::create()
	{
		return std::shared_ptr<ProtobufArenaBlock>(new ProtobufArenaBlock);
	}
}