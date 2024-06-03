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
		Block(const std::string& data);
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
		inline operator uint8_t* () { return mData; }
		inline operator const uint8_t* () const { return mData; }

		inline unsigned char* data(size_t startIndex) { assert(startIndex < mSize); return &mData[startIndex]; }
		inline const unsigned char* data(size_t startIndex) const { assert(startIndex < mSize); return &mData[startIndex]; }
		std::string convertToHex() const;
		std::string copyAsString() const;
		//! \return 0 if ok
		//!        -1 if bin is to small
		//!        -2 if hex is invalid
		static inline Block& fromHex(const std::string& hex) {
			return fromHex(hex.data(), hex.size());
		}
		static Block& fromHex(const char* hexString, size_t stringSize);

		bool isTheSame(const Block& b) const;
		inline bool operator == (const Block& b) const { return isTheSame(b); }
		inline bool operator != (const Block& b) const { return !isTheSame(b); }

	protected:
		void clear();
		size_t mSize;
		uint8_t* mData;

		static std::mutex mBlockStacksMutex;
		static std::map<size_t, BlockStack*> mBlockStacks;
	};

	typedef std::shared_ptr<Block> MemoryBlockPtr;
	typedef std::shared_ptr<const Block> ConstMemoryBlockPtr;
}

typedef memory::Block MemoryBin;


#endif //__GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H