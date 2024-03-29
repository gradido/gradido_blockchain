#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/Decay.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionBase.h"


#include "sodium.h"
#include <memory.h>
#include <exception>
#include <string>

#define _DEFAULT_PAGE_SIZE 10

MemoryBin::MemoryBin(uint32_t size)
	: mSize(size), mData(nullptr)
{
	mData = (unsigned char*)malloc(size);
}

MemoryBin::~MemoryBin()
{
	if (mData) {
		free(mData);
		mData = nullptr;
	}
}

std::unique_ptr<std::string> MemoryBin::convertToHex() const
{
	auto mm = MemoryManager::getInstance();

	uint32_t hexSize = mSize * 2 + 1;
	auto hexMem = mm->getMemory(hexSize);
	//char* hexString = (char*)malloc(hexSize);
	memset(*hexMem, 0, hexSize);
	sodium_bin2hex(*hexMem, hexSize, mData, mSize);
	std::unique_ptr<std::string> hex(new std::string((const char*)hexMem->data(), hexMem->size()));
	//	free(hexString);
	mm->releaseMemory(hexMem);

	return hex;
}

std::string MemoryBin::convertToHexString() const
{
	auto mm = MemoryManager::getInstance();

	uint32_t hexSize = mSize * 2 + 1;
	auto hexMem = mm->getMemory(hexSize);
	//char* hexString = (char*)malloc(hexSize);
	memset(*hexMem, 0, hexSize);
	sodium_bin2hex(*hexMem, hexSize, mData, mSize);
	std::string hex((const char*)hexMem->data(), hexMem->size());
	//	free(hexString);
	mm->releaseMemory(hexMem);

	return std::move(hex);
}

std::unique_ptr<std::string> MemoryBin::copyAsString() const
{
	return std::unique_ptr<std::string>(new std::string((const char*)mData, mSize));
}

int MemoryBin::convertFromHex(const std::string& hex)
{
	auto mm = MemoryManager::getInstance();
	size_t hexSize = hex.size();
	size_t binSize = (hexSize) / 2;
	if (binSize > mSize) {
		return -1;
	}
	memset(mData, 0, mSize);

	size_t resultBinSize = 0;

	if (0 != sodium_hex2bin(mData, binSize, hex.data(), hexSize, nullptr, &resultBinSize, nullptr)) {
		return -2;
	}
	return 0;
}

void MemoryBin::copyFromProtoBytes(const std::string& bytes)
{
	assert(bytes.size() == size());
	memcpy(mData, bytes.data(), bytes.size());
}

bool MemoryBin::isSame(const MemoryBin* b) const
{
	if (b->size() != size()) {
		return false;
	}
	return 0 == memcmp(data(), b->data(), size());
}

MemoryBin* MemoryBin::copy() const
{
	auto result = MemoryManager::getInstance()->getMemory(mSize);
	result->copyFrom(this);
	return result;
}


// *************************************************************

MemoryPageStack::MemoryPageStack(uint16_t size)
	: mSize(size)
{
	mMemoryBinStack.push(new MemoryBin(size));
}

MemoryPageStack::~MemoryPageStack()
{
	lock();
	while (mMemoryBinStack.size() > 0) {
		MemoryBin* memoryBin = mMemoryBinStack.top();
		mMemoryBinStack.pop();
		delete memoryBin;
	}
	unlock();
}

MemoryBin* MemoryPageStack::getMemory()
{
	std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);

	if (!mSize) {
		return nullptr;
	}
	if (mMemoryBinStack.size() == 0) {
		return new MemoryBin(mSize);
	}
	MemoryBin* memoryBin = mMemoryBinStack.top();
	mMemoryBinStack.pop();

	return memoryBin;
}
void MemoryPageStack::releaseMemory(MemoryBin* memory)
{
	if (!memory) return;
	std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);

	if (memory->size() != mSize) {
		throw MemoryManagerException("MemoryPageStack::releaseMemory wrong memory page stack", memory->size());
	}
	mMemoryBinStack.push(memory);
}

void MemoryPageStack::clear()
{
	lock();
	while (mMemoryBinStack.size() > 0) {
		MemoryBin* memoryBin = mMemoryBinStack.top();
		mMemoryBinStack.pop();
		delete memoryBin;
	}
	unlock();
}

// ***************** Mathe Memory *************************************************
MathMemory::MathMemory()
{
	mData = MemoryManager::getInstance()->getMathMemory();
}

MathMemory::~MathMemory()
{
	MemoryManager::getInstance()->releaseMathMemory(mData);
}

std::unique_ptr<MathMemory> MathMemory::create()
{
	return std::move(std::unique_ptr<MathMemory>(new MathMemory));
}

// ******************** google protobuf arena memory ********************************
ProtobufArenaMemory::ProtobufArenaMemory()
	: mArena(nullptr)
{
	mArena = MemoryManager::getInstance()->getProtobufArenaMemory();
}

ProtobufArenaMemory::~ProtobufArenaMemory()
{
	MemoryManager::getInstance()->releaseMemory(mArena);
	mArena = nullptr;
}

std::shared_ptr<ProtobufArenaMemory> ProtobufArenaMemory::create()
{
	return std::shared_ptr<ProtobufArenaMemory>(new ProtobufArenaMemory);
}

// ***********************************************************************************

MemoryManager* MemoryManager::getInstance()
{
	static MemoryManager only;
	return &only;
}


MemoryManager::MemoryManager()
{
	mMemoryPageStacks[0] = new MemoryPageStack(32); // pubkey
	mMemoryPageStacks[1] = new MemoryPageStack(64); // privkey
	mMemoryPageStacks[2] = new MemoryPageStack(65); // pubkey hex
	mMemoryPageStacks[3] = new MemoryPageStack(96); // privkey encrypted
	mMemoryPageStacks[4] = new MemoryPageStack(161); // privkey hex
	mMemoryPageStacks[5] = new MemoryPageStack(48); // word indices
}

MemoryManager::~MemoryManager()
{
	for (int i = 0; i < 5; i++) {
		delete mMemoryPageStacks[i];
		mMemoryPageStacks[i] = nullptr;
	}
	{
		std::scoped_lock<std::mutex> _lock(mMpfrMutex);
		while (mMpfrPtrStack.size() > 0) {
			mpfr_ptr mathMemory = mMpfrPtrStack.top();
			mMpfrPtrStack.pop();
			mpfr_clear(mathMemory);
			delete mathMemory;
		}
	}
	{
		std::scoped_lock _lock(mProtobufArenaMutex);
		while (mProtobufArenaStack.size() > 0) {
			auto arena = mProtobufArenaStack.top();
			mProtobufArenaStack.pop();
			delete arena;
		}
	}
}

int8_t MemoryManager::getMemoryStackIndex(uint16_t size) noexcept
{
	switch (size) {
	case 32: return 0;
	case 64: return 1;
	case 65: return 2;
	case 96: return 3;
	case 161: return 4;
	case 48: return 5;
	default: return -1;
	}
	return -1;
}




MemoryBin* MemoryManager::getMemory(uint32_t size)
{
	assert(size == (uint32_t)((uint16_t)size));
	assert(size > 0);
	auto index = getMemoryStackIndex(size);
	
	if (index < 0) {
		return new MemoryBin(size);
	}
	else {
		assert(mMemoryPageStacks[index]);
		return mMemoryPageStacks[index]->getMemory();
	}
	return nullptr;
}

void MemoryManager::releaseMemory(MemoryBin* memory) noexcept
{
	if (!memory) return;
	auto index = getMemoryStackIndex(memory->size());
	if (index < 0 || !mMemoryPageStacks[index]) {
		delete memory;
	}
	else {
		try {
			mMemoryPageStacks[index]->releaseMemory(memory);
		}
		catch (...) {
			delete memory;
		}
	}
}

mpfr_ptr MemoryManager::getMathMemory()
{
	mpfr_ptr mathMemory;
	std::scoped_lock<std::mutex> _lock(mMpfrMutex);
	if (mMpfrPtrStack.size()) {
		mathMemory = mMpfrPtrStack.top();
		mMpfrPtrStack.pop();
	} else {
		mathMemory = new mpfr_t;
	}
	if (!mActiveMpfrs.insert(mathMemory).second) {
		assert(false && "[MemoryManager::getMathMemory] try to return mathe memory already in use");
	}
	mpfr_init2(mathMemory, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	mpfr_set_ui(mathMemory, 0, gDefaultRound);
	return mathMemory;
}
void MemoryManager::releaseMathMemory(mpfr_ptr ptr)
{
	if (!ptr) return;
	std::scoped_lock<std::mutex> _lock(mMpfrMutex);

	if (ptr->_mpfr_prec != MAGIC_NUMBER_AMOUNT_PRECISION_BITS) {
		throw MemoryManagerException("wrong precision", ptr->_mpfr_prec);
	}
	mMpfrPtrStack.push(ptr);
	if (!mActiveMpfrs.erase(ptr)) {
		assert(false && "[MemoryManager::getMathMemory] try to remove math memory already removed");
	}
}

void MemoryManager::clearProtobufMemory()
{
	std::scoped_lock _lock(mProtobufArenaMutex);
	while (mProtobufArenaStack.size() > 0) {
		auto arena = mProtobufArenaStack.top();
		mProtobufArenaStack.pop();
		delete arena;
	}
}

void MemoryManager::clearMathMemory()
{
	std::scoped_lock<std::mutex> _lock(mMpfrMutex);
	while (mMpfrPtrStack.size() > 0) {
		mpfr_ptr mathMemory = mMpfrPtrStack.top();
		mMpfrPtrStack.pop();
		mpfr_clear(mathMemory);
		delete mathMemory;
	}
}

void MemoryManager::clearMemory()
{
	for (int i = 0; i < 6; i++) {
		mMemoryPageStacks[i]->clear();
	}
}

google::protobuf::Arena* MemoryManager::getProtobufArenaMemory()
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
		assert(false && "[MemoryManager::getProtobufArenaMemory] try to return protobuf memory already in use");
	}

	return arena;
}
void MemoryManager::releaseMemory(google::protobuf::Arena* memory)
{
	if (!memory) return;
	std::scoped_lock _lock(mProtobufArenaMutex);

	if (!mActiveProtobufArenas.erase(memory)) {
		assert(false && "[MemoryManager::releaseMemory] try to remove protobuf arena memory already removed");
	}

	if (memory->SpaceAllocated() > 7168) {
		//printf("delete protobuf arena space allocated: %d, still active: %d\n", memory->SpaceAllocated(), mActiveProtobufArenas.size());
		delete memory;
	}
	else {
		mProtobufArenaStack.push(memory);
		auto usedSpaceBefore = memory->SpaceUsed();
		auto usedSpace = memory->Reset();
		if (usedSpace > 7168) {
			int zahl = 1;
		}
		//printf("release protobuf arena used size: %d, still active: %d\n", usedSpace, mActiveProtobufArenas.size());
	}

}


// ******************* Exception **************************
MemoryManagerException::MemoryManagerException(const char* what, uint32_t size /* = 0*/)
	: GradidoBlockchainException(what), mSize(size)
{

}

std::string MemoryManagerException::getFullString() const
{
	if (mSize) {
		std::string result;
		size_t resultSize = strlen(what());
		resultSize += 3 + 14;
		result = what();
		result += " with size: " + std::to_string(mSize);
		return result;
	}
	else {
		return what();
	}
}
