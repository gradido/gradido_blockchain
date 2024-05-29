#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H

#include "BlockStack.h"
#include "gradido_blockchain/export.h"

#include <map>
#include <vector>
#include <cassert>

namespace memory {

	class GRADIDOBLOCKCHAIN_EXPORT Block
	{
	public:
		// call getMemory of MemoryManager
		Block(size_t size);
		// copy data
		Block(size_t size, const unsigned char* data);
		Block(const std::vector<unsigned char>& data);
		// copy
		Block(Block& other);
		Block(const Block& other);
		// move
		Block(Block&& other) noexcept;
		// also move 
		Block& operator=(Block&& other) noexcept;

		~Block();

		inline size_t size() const { return static_cast<size_t>(mSize); }

		inline uint8_t* data() { return mData; }
		inline const uint8_t* data() const { return mData; }

		inline unsigned char* data(size_t startIndex) { assert(startIndex < mSize); return &mData[startIndex]; }
		inline const unsigned char* data(size_t startIndex) const { assert(startIndex < mSize); return &mData[startIndex]; }
		inline std::unique_ptr<std::string> convertToHex() const { return std::make_unique<std::string>(convertToHexString()); }
		std::string convertToHexString() const;
		std::unique_ptr<std::string> copyAsString() const;
		//! \return 0 if ok
		//!        -1 if bin is to small
		//!        -2 if hex is invalid
		int convertFromHex(const std::string& hex);
		static Block& fromHex(const std::string& hex);
		void copyFromProtoBytes(const std::string& bytes);
		inline void copyFrom(const Block* origin) { assert(mSize == origin->size()); memcpy(mData, origin->data(), mSize); }
		inline void copyFrom(const unsigned char* origin) { memcpy(mData, origin, mSize); }

		bool isSame(const Block* b) const;

	protected:
		void clear();
		size_t mSize;
		uint8_t* mData;

		static std::mutex mBlockStacksMutex;
		static std::map<size_t, BlockStack*> mBlockStacks;
	};
}

typedef memory::Block MemoryBin;
typedef std::shared_ptr<memory::Block> MemoryBlockPtr;
typedef std::shared_ptr<const memory::Block> ConstMemoryBlockPtr;

#endif //__GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H