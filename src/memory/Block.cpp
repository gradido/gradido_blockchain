#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "sodium.h"

namespace memory {

	std::mutex Block::mBlockStacksMutex;
	std::map<size_t, BlockStack*> Block::mBlockStacks;

	Block::Block(size_t size)
		: mSize(size), mData(nullptr)
	{
		assert(size > 0);
		std::lock_guard _lock(mBlockStacksMutex);
		auto it = mBlockStacks.find(size);
		if (it == mBlockStacks.end()) {
			it = mBlockStacks.insert({ size, new BlockStack(size) }).first;
		}
		mData = it->second->getBlock();
	}

	Block::Block(size_t size, const unsigned char* data)
		: Block(size)
	{
		memcpy(mData, data, size);
	}

	Block::Block(const std::vector<unsigned char>& data)
		: Block(data.size(), data.data()) 
	{
	
	}
	Block::Block(const std::string& data)
		: Block(data.size(), (const unsigned char*)data.data())
	{

	}
	// copy
	Block::Block(Block& other)
		: Block(other.size(), other.data())
	{

	}

	Block::Block(const Block& other)
		: Block(other.size(), other.data())
	{

	}
	// move
	Block::Block(Block&& other) noexcept
		: mSize(other.size()), mData(other.data())
	{
		other.mSize = 0;
		other.mData = nullptr;
	}
	// also move 
	Block& Block::operator=(Block&& other) noexcept
	{
		clear();
		mSize = other.mSize;
		mData = other.mData;
		other.mSize = 0;
		other.mData = nullptr;
		return *this;
	}

	Block::~Block()
	{
		clear();
	}

	void Block::clear()
	{
		if (mData) {
			std::lock_guard _lock(mBlockStacksMutex);
			auto it = mBlockStacks.find(mSize);
			if (it != mBlockStacks.end()) {
				it->second->releaseBlock(mData);
			}
			else {
				free(mData);
			}
			mData = nullptr;
			mSize = 0;
		}
	}

	std::string Block::convertToHex() const
	{
		uint32_t hexSize = mSize * 2 + 1;
		Block hexMem(hexSize);
		sodium_bin2hex((char*)hexMem.data(), hexSize, mData, mSize);
		std::string hex((char*)hexMem.data(), hexMem.size()-1);
		return hex;
	}

	std::string Block::copyAsString() const
	{
		return { (char*)mData, mSize };
	}

	Block& Block::fromHex(const char* hexString, size_t stringSize)
	{
		size_t binSize = (stringSize) / 2;
		Block result(binSize);
	
		size_t resultBinSize = 0;
		if (0 != sodium_hex2bin(result.data(), binSize, hexString, stringSize, nullptr, &resultBinSize, nullptr)) {
			throw GradidoInvalidHexException("invalid hex for Block::fromHex", hexString);
		}
		return result;
	}

	bool Block::isTheSame(const Block& b) const
	{
		if (b.size() != size()) {
			return false;
		}
		return 0 == memcmp(mData, b, size());
	}

	// *************** Cached Memory Block *************************
	// call getMemory of MemoryManager
	


}