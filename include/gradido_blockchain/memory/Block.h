#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H

#include "VectorCacheAllocator.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/export.h"

#include "sodium.h"

#include <map>
#include <span>
#include <vector>
#include <cassert>
#include <cstring>
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
		Block(PublicKey publicKey);
		// copy
		Block(Block& other);
		Block(const Block& other);
		// move
		Block(Block&& other) noexcept;
		// also move
		Block& operator=(Block&& other) noexcept;
		// also copy
		Block& operator=(const Block& other);

		~Block();

		inline size_t size() const { return static_cast<size_t>(mSize); }
		inline uint8_t* data() { mShortHash.octet = 0;  return mData; }
		inline const uint8_t* data() const { return mData; }
		inline std::span<std::byte> span() const { return { reinterpret_cast<std::byte*>(mData), mSize }; }
		inline operator uint8_t* () { mShortHash.octet = 0; return mData; }
		inline operator const uint8_t* () const { return mData; }
		inline unsigned char* data(size_t startIndex) { assert(startIndex < mSize); mShortHash.octet = 0; return &mData[startIndex]; }
		inline const unsigned char* data(size_t startIndex) const { assert(startIndex < mSize); return &mData[startIndex]; }
		inline SignatureOctet hash() const { return mShortHash; }

		uint8_t& operator [](int idx) { mShortHash.octet = 0;  return mData[idx];}
		uint8_t  operator [](int idx) const { return mData[idx];}
		std::string convertToHex() const;
		std::string convertToBase64(int variant = sodium_base64_VARIANT_ORIGINAL) const;
		std::string copyAsString() const;
		std::vector<uint8_t> copyAsVector() const;
		//! calculate generichash with libsodium using BLAKE2b algorithm
		//! https://libsodium.gitbook.io/doc/hashing/generic_hashing
		Block calculateHash() const;
		static inline Block fromHex(const std::string& hex) {
			return fromHex(hex.data(), hex.size());
		}
		static inline Block fromHex(std::string_view hex) {
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
		// short hash for speeding up comparisations
		SignatureOctet mShortHash;
	};

	using BlockPtr = std::shared_ptr<Block>;
	using ConstBlockPtr = std::shared_ptr<const Block>;

	struct GRADIDOBLOCKCHAIN_EXPORT ConstBlockPtrComparator {
		bool operator()(ConstBlockPtr a, ConstBlockPtr b) const
		{
			// Compare based on string length 
			return *a < *b;
		}
	};
	struct GRADIDOBLOCKCHAIN_EXPORT ConstBlockPtrHash
	{
		size_t operator()(const ConstBlockPtr& s) const noexcept;
	};

	struct GRADIDOBLOCKCHAIN_EXPORT ConstBlockPtrEqual
	{
		bool operator()(const ConstBlockPtr& a, const ConstBlockPtr& b) const noexcept {
			if (!a && !b) { return true; }
			if (!a || !b) { return false; }
			return a->isTheSame(b);
		}
	};
}

typedef memory::Block MemoryBin;


#endif //__GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_H
