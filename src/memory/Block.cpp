#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Manager.h"

#include "loguru/loguru.hpp"

namespace memory {
	
	Block::Block(size_t size)
		: mSize(size), mData(Manager::getInstance()->getBlock(size))
	{		
	}

	Block::Block(size_t size, const unsigned char* data)
		: Block(size)
	{
		if (!size) return;
		mShortHash = BlockKey(size, data);
		memcpy(mData, data, size);
	}

	Block::Block(const std::vector<unsigned char>& data)
		: Block(data.size(), data.data()) 
	{
	}

	Block::Block(std::span<std::byte> data) 
		: Block(data.size(), reinterpret_cast<const unsigned char*>(data.data()))
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
		: mSize(other.size()), mData(other.data()), mShortHash(other.mShortHash)
	{
		other.mSize = 0;
		other.mData = nullptr;
		other.mShortHash.shortHash = 0;
	}
	// also move 
	Block& Block::operator=(Block&& other) noexcept
	{
		clear();
		mSize = other.mSize;
		mData = other.mData;
		mShortHash = other.mShortHash;
		other.mSize = 0;
		other.mData = nullptr;
		other.mShortHash.shortHash = 0;
		return *this;
	}
	// also copy
	Block& Block::operator=(const Block& other) {
		clear();
		mSize = other.mSize;
		if (!mSize) {
			if (other.mData) {
				LOG_F(WARNING, "Block::operator= (copy) called with size 0 but other has data");
			}
			return *this;
		}
		mData = Manager::getInstance()->getBlock(other.mSize);
		if (!mData) throw std::bad_alloc();
		memcpy(mData, other.mData, mSize);
		mShortHash = other.mShortHash;
		return *this;
	}

	Block::~Block()
	{
		clear();
	}

	void Block::clear()
	{
		if (mData) {
			Manager::getInstance()->releaseBlock(mSize, mData);
			mData = nullptr;
			mSize = 0;
			mShortHash.shortHash = 0;
		}
	}

	std::string Block::convertToHex() const
	{
		if (!mSize) return "";
		uint32_t hexSize = mSize * 2 + 1;
		Block hexMem(hexSize);
		sodium_bin2hex((char*)hexMem.data(), hexSize, mData, mSize);
		std::string hex((char*)hexMem.data(), hexMem.size()-1);
		return hex;
	}

	std::string Block::convertToBase64(int variant/* = sodium_base64_VARIANT_ORIGINAL*/) const
	{
		if (!mSize) return "";
		size_t encodedSize = sodium_base64_encoded_len(mSize, variant);
		memory::Block base64(encodedSize);

		if (nullptr == sodium_bin2base64((char*)base64.data(), encodedSize, mData, mSize, variant)) {
			return "";
		}

		std::string base64String((const char*)base64.data(), encodedSize - 1);
		return base64String;
	}

	std::string Block::copyAsString() const
	{
		if (!mSize) return "";
		return { (char*)mData, mSize };
	}

	std::vector<uint8_t> Block::copyAsVector() const
	{
		if (!mSize) return {};
		return { mData, mData + mSize };
	}

	Block Block::calculateHash() const
	{
		memory::Block hash(crypto_generichash_BYTES);
		crypto_generichash(hash, crypto_generichash_BYTES, mData, mSize, nullptr, 0);
		return hash;
	}

	Block Block::fromHex(const char* hexString, size_t stringSize)
	{
		size_t binSize = (stringSize) / 2;
		// invalid hex size
		if (binSize * 2 != stringSize) {
			throw GradidoInvalidHexException("invalid hex size Block::fromHex", hexString);
		}
		Block result(binSize);
	
		size_t resultBinSize = 0;
		if (0 != sodium_hex2bin(result.data(), binSize, hexString, stringSize, nullptr, &resultBinSize, nullptr)) {
			throw GradidoInvalidHexException("invalid hex for Block::fromHex", hexString);
		}
		return result;
	}

	Block Block::fromBase64(const char* base64String, size_t size, int variant /*= sodium_base64_VARIANT_ORIGINAL*/)
	{
		size_t binSize = (size / 4) * 3;
		
		memory::Block bin(binSize);
		size_t resultBinSize = 0;
		const char* firstInvalidByte = nullptr;
		auto convertResult = sodium_base642bin(bin, binSize, base64String, size, nullptr, &resultBinSize, &firstInvalidByte, variant);
		if (0 != convertResult) {
			throw GradidoInvalidBase64Exception("invalid base64", base64String, firstInvalidByte - base64String);
		}
		if (resultBinSize < binSize) {
			memory::Block bin_real(resultBinSize, bin);
			return bin_real;
		}

		return bin;
	}

	bool Block::isTheSame(const Block& b) const
	{
		if (mShortHash != b.mShortHash) {
			return false;
		}
		if (b.size() != size()) {
			return false;
		}
		if (!size()) { return true; }
		return 0 == memcmp(mData, b, size());
	}
	// is for loop faster or memcmp with zero filled buffer? (overhead for filling buffer or keeping static filled buffer in memory for each size, or maximal size?)
	bool Block::isEmpty() const
	{
		for (auto i = 0; i < mSize; i++) {
			if (mData[i] != 0) {
				return false;
			}
		}
		return true;
	}

	// *************** Cached Memory Block *************************
	// call getMemory of MemoryManager
	


}