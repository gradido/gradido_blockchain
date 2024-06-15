#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H

#include "BlockStack.h"
#include "VectorCacheAllocator.h"
#include "gradido_blockchain/export.h"

#include "sodium.h"

#include <map>
#include <span>
#include <vector>
#include <cassert>
#include <memory>

namespace memory {

	class GRADIDOBLOCKCHAIN_EXPORT Block
	{
	public:
		// call getMemory of MemoryManager
		Block(size_t size);
		// copy data
		Block(size_t size, const unsigned char* data);
		Block(const std::vector<unsigned char>& data);
		Block(std::span<std::byte> data);
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
		inline std::span<std::byte> span() const { return { reinterpret_cast<std::byte*>(mData), mSize }; }
		inline operator uint8_t* () { return mData; }
		inline operator const uint8_t* () const { return mData; }
		inline unsigned char* data(size_t startIndex) { assert(startIndex < mSize); return &mData[startIndex]; }
		inline const unsigned char* data(size_t startIndex) const { assert(startIndex < mSize); return &mData[startIndex]; }
		std::string convertToHex() const;
		std::string convertToBase64(int variant = sodium_base64_VARIANT_ORIGINAL) const;
		std::string copyAsString() const;
		std::vector<uint8_t> copyAsVector() const;
		//! \return 0 if ok
		//!        -1 if bin is to small
		//!        -2 if hex is invalid
		static inline Block fromHex(const std::string& hex) {
			return fromHex(hex.data(), hex.size());
		}
		static Block fromHex(const char* hexString, size_t stringSize);
		static inline Block fromBase64(const std::string& base64, int variant = sodium_base64_VARIANT_ORIGINAL) {
			return fromBase64(base64.data(), base64.size(), variant);
		}
		static Block fromBase64(const char* base64String, size_t size, int variant /*= sodium_base64_VARIANT_ORIGINAL*/);

		bool isTheSame(const Block& b) const;
		inline bool isTheSame(const std::shared_ptr<const Block> b) const {
			if (!b) return false;
			return isTheSame(*b);
		}
		//! \return true if all values are 0, for mSize = 4 => 0000
		bool isEmpty() const;
		inline bool operator == (const Block& b) const { return isTheSame(b); }
		inline bool operator != (const Block& b) const { return !isTheSame(b); }
		// compare operator for std container ordering
		bool operator < (const Block& b) const
		{
			if (mSize != b.mSize) {
				return mSize < b.mSize;
			}
			return std::memcmp(mData, b.mData, mSize) < 0;
		}

	protected:
		void clear();
		size_t mSize;
		uint8_t* mData;
	};

	typedef std::shared_ptr<Block> BlockPtr;
	typedef std::shared_ptr<const Block> ConstBlockPtr;
}

typedef memory::Block MemoryBin;


#endif //__GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H
