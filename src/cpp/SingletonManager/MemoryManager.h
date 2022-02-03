/*!
*
* \author: einhornimmond
*
* \date: 01.11.19
*
* \brief: manage memory blocks to reduce dynamic memory allocation for preventing memory fragmentation,
* specially for key memory blocks of 32 and 64 byte 
*/

#ifndef GRADIDO_LOGIN_SERVER_SINGLETON_MANAGER_MEMORY_MANAGER_H
#define GRADIDO_LOGIN_SERVER_SINGLETON_MANAGER_MEMORY_MANAGER_H

//#include "../lib/DRMemoryList.h"
#include "../lib/MultithreadContainer.h"

#include <list>
#include <stack>
#include <assert.h>
#include <mutex>

#define MEMORY_MANAGER_PAGE_SIZE 10

class MemoryPageStack;
class MemoryManager;

class MemoryBin 
{
	friend MemoryPageStack;
	friend MemoryManager;
public: 
	
	inline operator unsigned char*() { return mData; }
	inline operator char*() { return (char*)mData; }
	inline operator void* () { return mData; }
	inline operator const unsigned char*() const { return mData; }

	inline size_t size() const { return static_cast<size_t>(mSize); }
	inline operator size_t() const { return static_cast<size_t>(mSize); }

	inline unsigned char* data() { return mData; }
	inline const unsigned char* data() const { return mData; }

	inline unsigned char* data(size_t startIndex) { assert(startIndex < mSize); return &mData[startIndex]; }
	inline const unsigned char* data(size_t startIndex) const { assert(startIndex < mSize); return &mData[startIndex]; }
	std::unique_ptr<std::string> convertToHex() const;
	std::unique_ptr<std::string> copyAsString() const;
	//! \return 0 if ok
	//!        -1 if bin is to small
	//!        -2 if hex is invalid
	int convertFromHex(const std::string& hex);
	void copyFromProtoBytes(const std::string& bytes);
	inline void copyFrom(MemoryBin* origin) { assert(mSize == origin->size()); memcpy(mData, *origin, mSize); }

	bool isSame(const MemoryBin* b) const;

protected:
	MemoryBin(uint32_t size);
	~MemoryBin();

	uint16_t mSize;
	unsigned char* mData;

};

class MemoryPageStack : protected UniLib::lib::MultithreadContainer
{
public:
	MemoryPageStack(uint16_t size);
	~MemoryPageStack();

	MemoryBin* getMemory();
	void releaseMemory(MemoryBin* memory);

protected:
	std::stack<MemoryBin*> mMemoryBinStack;
	uint16_t mSize;
};

class MemoryManager
{
public:
	~MemoryManager();

	static MemoryManager* getInstance();

	MemoryBin* getMemory(uint32_t size);
	void releaseMemory(MemoryBin* memory) noexcept;
	
protected:

	int8_t getMemoryStackIndex(uint16_t size) noexcept;

	MemoryManager();
	MemoryPageStack* mMemoryPageStacks[6];
};


class MemoryManagerException : public GradidoBlockchainException
{
public:
	MemoryManagerException(const char* what, uint32_t size = 0);
	std::string getFullString() const;

protected:
	uint32_t mSize;

};


#endif //GRADIDO_LOGIN_SERVER_SINGLETON_MANAGER_MEMORY_MANAGER_H