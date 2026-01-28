#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/Manager.h"

#include "loguru/loguru.hpp"

using gradido::data::compact::PublicKeyIndex;
using std::string;
using std::vector, std::byte;

namespace memory {

	Block::Block(size_t size)
		: mSize(size), mData(Manager::getInstance()->getBlock(size))
	{
	}

	Block::Block(size_t size, const unsigned char* data)
		: Block(size)
	{
		if (!size) return;
		mShortHash = SignatureOctet(data, size);
		memcpy(mData, data, size);
	}

	Block::Block(const vector<unsigned char>& data)
		: Block(data.size(), data.data())
	{
	}

	Block::Block(std::span<byte> data)
		: Block(data.size(), reinterpret_cast<const unsigned char*>(data.data()))
	{

	}

	Block::Block(const string& data)
		: Block(data.size(), (const unsigned char*)data.data())
	{

	}

	Block::Block(PublicKey publicKey)
		: Block(publicKey.size(), publicKey.data())
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
		other.mShortHash.octet = 0;
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
		other.mShortHash.octet = 0;
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
			mShortHash.octet = 0;
		}
	}

	string Block::convertToHex() const
	{
		if (!mSize) return "";
		uint32_t hexSize = mSize * 2 + 1;
		Block hexMem(hexSize);
		sodium_bin2hex((char*)hexMem.data(), hexSize, mData, mSize);
		string hex((char*)hexMem.data(), hexMem.size()-1);
		return hex;
	}

	string Block::convertToBase64(int variant/* = sodium_base64_VARIANT_ORIGINAL*/) const
	{
		if (!mSize) return "";
		size_t encodedSize = sodium_base64_encoded_len(mSize, variant);
		Block base64(encodedSize);

		if (nullptr == sodium_bin2base64((char*)base64.data(), encodedSize, mData, mSize, variant)) {
			return "";
		}

		string base64String((const char*)base64.data(), encodedSize - 1);
		return base64String;
	}

	string Block::copyAsString() const
	{
		if (!mSize) return "";
		return { (char*)mData, mSize };
	}

	vector<uint8_t> Block::copyAsVector() const
	{
		if (!mSize) return {};
		return { mData, mData + mSize };
	}

	Block Block::calculateHash() const
	{
		memory::Block hash(crypto_generichash_BYTES);
		crypto_generichash(hash, crypto_generichash_BYTES, mData, mSize, nullptr, 0);
		hash.mShortHash = SignatureOctet(mData, mSize);
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
		result.mShortHash = SignatureOctet(result.data(), result.size());
		return result;
	}

	Block Block::fromBase64(const char* base64String, size_t size, int variant /*= sodium_base64_VARIANT_ORIGINAL*/)
	{
		size_t binSize = (size / 4) * 3;
		
		Block bin(binSize);
		size_t resultBinSize = 0;
		const char* firstInvalidByte = nullptr;
		auto convertResult = sodium_base642bin(bin, binSize, base64String, size, nullptr, &resultBinSize, &firstInvalidByte, variant);
		if (0 != convertResult) {
			throw GradidoInvalidBase64Exception("invalid base64", base64String, firstInvalidByte - base64String);
		}
		if (resultBinSize < binSize) {
			Block bin_real(resultBinSize, bin);
			return bin_real;
		}
		bin.mShortHash = SignatureOctet(bin.data(), bin.size());
		return bin;
	}
	bool Block::isTheSame(PublicKeyIndex publicKeyIndex) const
	{
		if (mSize != 32) {
			return false;
		}
		auto publicKey = publicKeyIndex.getRawKey();
		return memcmp(publicKey.data(), mData, 32) == 0;
	}

	bool Block::isTheSame(const PublicKey& publicKey) const
	{
		if (mSize != 32) {
			return false;
		}
		return memcmp(publicKey.data(), mData, 32) == 0;
	}

	bool Block::isTheSame(const Block& b) const
	{
		if (!mShortHash.empty() && !b.mShortHash.empty() && mShortHash != b.mShortHash) {
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


	// named hash function/struct and comparisation for explicit using in unordered_map
	size_t ConstBlockPtrHash::operator()(const ConstBlockPtr& s) const noexcept
	{
		if (!s) { return 0; }
		int64_t octet = s->hash().octet;
		if (!octet) { octet = SignatureOctet(*s).octet; }
		return std::hash<int64_t>()(octet);
	}
}


