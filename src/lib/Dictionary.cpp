#include "gradido_blockchain/lib/Dictionary.h"
#include "gradido_blockchain/memory/Block.h"
/*#include "Exceptions.h"
#include "../ServerGlobals.h"
#include "../SystemExceptions.h"
#include "../model/files/FileExceptions.h"
*/
#include "loguru/loguru.hpp"

using std::shared_lock, std::unique_lock;
using memory::Block, memory::ConstBlockPtr;

Dictionary::Dictionary(const char* name)
	: mName(name), mLastIndex(0)
{
}

Dictionary::~Dictionary()
{
	reset();
}

void Dictionary::reset()
{
	mBinaryIndexLookup.clear();
	mIndexBinaryLookup.clear();
	mLastIndex = 0;
}

bool Dictionary::addBinaryIndex(ConstBlockPtr binary, uint32_t index)
{
	if (hasBinaryIndexPair(binary, index)) {
		LOG_F(WARNING, "try to add binary index pair which already exist");
		return false;
	}
	if (mLastIndex + 1 != index) {
		throw DictionaryInvalidNewKeyException(
			"addValueKeyPair called with invalid new key",
			mName.data(),
			index,
			mLastIndex
		);
	}
	mLastIndex = index;
	mBinaryIndexLookup.insert({ SignatureOctet(*binary), { binary, index } });
	mIndexBinaryLookup.insert({ index, binary });

	return true;
}

uint32_t Dictionary::getIndexForBinary(ConstBlockPtr binary)  const
{
	auto result = findIndexForBinary(binary);
	if (!result) {
		throw DictionaryNotFoundException(
			"binary not found in dictionary", 
			mName.data(), 
			binary->convertToHex().data()
		);
	}
	return result;
}

ConstBlockPtr Dictionary::getBinaryForIndex(uint32_t index) const
{
	auto it = mIndexBinaryLookup.find(index);

	if(it == mIndexBinaryLookup.end()) {
		throw DictionaryNotFoundException(
			"index not found in dictionary", 
			mName.data(),
			std::to_string(index).c_str()
		);
	}
	return it->second;
}
bool Dictionary::hasIndex(uint32_t index) const
{
	auto it = mIndexBinaryLookup.find(index);
	return it != mIndexBinaryLookup.end();
}

uint32_t Dictionary::getOrAddIndexForBinary(ConstBlockPtr binary)
{

	auto index = findIndexForBinary(binary);
	if (!index) {
		index = ++mLastIndex;
		mBinaryIndexLookup.insert({ SignatureOctet(*binary), { binary, index } });
		mIndexBinaryLookup.insert({ index, binary });
	}
	return index;
}

bool Dictionary::hasBinaryIndexPair(ConstBlockPtr binary, uint32_t index) const
{
	auto it = mIndexBinaryLookup.find(index);
	if (it == mIndexBinaryLookup.end()) {
		return false;
	}
	if (it->second->isTheSame(binary)) {
		return true;
	}
	return false;
}
	 
uint32_t Dictionary::findIndexForBinary(ConstBlockPtr binary) const
{
	if (!binary || binary->isEmpty()) {
		throw GradidoNodeInvalidDataException("binary is empty");
	}
	
	auto itRange = mBinaryIndexLookup.equal_range(SignatureOctet(*binary));
	for (auto& it = itRange.first; it != itRange.second; ++it) {
		if (it->second.first->isTheSame(binary)) {
			return it->second.second;
		}
	}
	return 0;
}

void DictionaryThreadsafe::reset()
{ 
	unique_lock _lock(mSharedMutex);
	Dictionary::reset();
}

bool DictionaryThreadsafe::addBinaryIndex(ConstBlockPtr binary, uint32_t index)
{
	unique_lock _lock(mSharedMutex);
	return Dictionary::addBinaryIndex(binary, index);
}

uint32_t DictionaryThreadsafe::getIndexForBinary(ConstBlockPtr binary)  const
{
	shared_lock _lock(mSharedMutex);
	return Dictionary::getIndexForBinary(binary);
}

ConstBlockPtr DictionaryThreadsafe::getBinaryForIndex(uint32_t index) const
{
	shared_lock _lock(mSharedMutex);
	return Dictionary::getBinaryForIndex(index);
}

bool DictionaryThreadsafe::hasIndex(uint32_t index) const
{
	shared_lock _lock(mSharedMutex);
	return Dictionary::hasIndex(index);
}

uint32_t DictionaryThreadsafe::getOrAddIndexForBinary(ConstBlockPtr binary)
{
	unique_lock _lock(mSharedMutex);
	return Dictionary::getOrAddIndexForBinary(binary);
}


DictionaryException::DictionaryException(const char* what, const char* dictionaryName) noexcept
	: GradidoBlockchainException(what), mDictionaryName(dictionaryName)
{

}



DictionaryNotFoundException::DictionaryNotFoundException(const char* what, const char* dictionaryName, const char* key) noexcept
	: DictionaryException(what, dictionaryName), mKey(key)
{

}

std::string DictionaryNotFoundException::getFullString() const
{
	std::string result = what();
	result += ", dictionary name: " + mDictionaryName;
	result += ", key: " + mKey;
	return result;
}

DictionaryInvalidNewKeyException::DictionaryInvalidNewKeyException(const char* what, const char* dictionaryName, uint32_t newKey, uint32_t oldKey) noexcept
	: DictionaryException(what, dictionaryName), mNewKey(newKey), mOldKey(oldKey)
{

}

std::string DictionaryInvalidNewKeyException::getFullString() const
{
	std::string result = what();
	result += ", dictionary name: " + mDictionaryName;
	result += ", current key: " + std::to_string(mOldKey);
	result += ", new key: " + std::to_string(mNewKey);
	return result;
}