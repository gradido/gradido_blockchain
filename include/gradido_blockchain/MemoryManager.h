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

#include "gradido_blockchain/lib/MultithreadContainer.h"
#include <google/protobuf/arena.h>

#include "proto/gradido/GradidoBlock.pb.h"
#include "proto/gradido/GradidoTransaction.pb.h"
#include "proto/gradido/TransactionBody.pb.h"

#include <list>
#include <stack>
#include <set>
#include <assert.h>
#include <mutex>
#include <memory>
#include <mpfr.h>

#define MEMORY_MANAGER_PAGE_SIZE 10

// MAGIC NUMBER: Define how many bits are used for amount calculation
#define MAGIC_NUMBER_AMOUNT_PRECISION_BITS 128

class MemoryPageStack;
class MemoryManager;



class GRADIDOBLOCKCHAIN_EXPORT MemoryBin
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
	std::string convertToHexString() const;
	std::unique_ptr<std::string> copyAsString() const;
	//! \return 0 if ok
	//!        -1 if bin is to small
	//!        -2 if hex is invalid
	int convertFromHex(const std::string& hex);
	void copyFromProtoBytes(const std::string& bytes);
	inline void copyFrom(const MemoryBin* origin) { assert(mSize == origin->size()); memcpy(mData, *origin, mSize); }
	inline void copyFrom(const unsigned char* origin) { memcpy(mData, origin, mSize); }
	MemoryBin* copy() const; 

	bool isSame(const MemoryBin* b) const;

protected:
	MemoryBin(uint32_t size);
	~MemoryBin();

	uint16_t mSize;
	unsigned char* mData;

};

class MemoryPageStack : protected MultithreadContainer
{
public:
	MemoryPageStack(uint16_t size);
	~MemoryPageStack();

	MemoryBin* getMemory();
	void releaseMemory(MemoryBin* memory);

	void clear();

protected:
	std::stack<MemoryBin*> mMemoryBinStack;
	const uint16_t mSize;
};

class GRADIDOBLOCKCHAIN_EXPORT MathMemory
{
public:
	~MathMemory();

	static std::unique_ptr<MathMemory> create();
	mpfr_ptr getData() { return mData; }
	inline operator mpfr_ptr() { return mData; }

protected:
	MathMemory();

	mpfr_ptr mData;
};

class GRADIDOBLOCKCHAIN_EXPORT ProtobufArenaMemory
{
public: 
	~ProtobufArenaMemory();
	inline operator google::protobuf::Arena* () { return mArena; }
	static std::shared_ptr<ProtobufArenaMemory> create();
	inline uint64_t getUsedSpace() const { return mArena->SpaceUsed(); }
	inline uint64_t getAllocatedSpace() const { return mArena->SpaceAllocated(); }
protected:
	ProtobufArenaMemory();

	google::protobuf::Arena* mArena;
};

// TODO: Add handling for mpfr pointer
// TODO: Add handling for protobuf messages

class GRADIDOBLOCKCHAIN_EXPORT MemoryManager
{
	friend class ProtobufArenaMemory;
public:
	~MemoryManager();

	static MemoryManager* getInstance();

	MemoryBin* getMemory(uint32_t size);
	void releaseMemory(MemoryBin* memory) noexcept;

	mpfr_ptr getMathMemory();
	void releaseMathMemory(mpfr_ptr ptr);

	void clearProtobufMemory();
	void clearMathMemory();
	void clearMemory();

protected:

	google::protobuf::Arena* getProtobufArenaMemory();
	void releaseMemory(google::protobuf::Arena* memory);

	int8_t getMemoryStackIndex(uint16_t size) noexcept;

	MemoryManager();
	MemoryPageStack* mMemoryPageStacks[6];

	std::mutex mMpfrMutex;
	std::stack<mpfr_ptr> mMpfrPtrStack;
	std::set<mpfr_ptr> mActiveMpfrs;

	std::mutex mProtobufArenaMutex;
	std::stack<google::protobuf::Arena*> mProtobufArenaStack;
	std::set<google::protobuf::Arena*> mActiveProtobufArenas;
};


class GRADIDOBLOCKCHAIN_EXPORT MemoryManagerException : public GradidoBlockchainException
{
public:
	MemoryManagerException(const char* what, uint32_t size = 0);
	std::string getFullString() const;

protected:
	uint32_t mSize;

};


#endif //GRADIDO_LOGIN_SERVER_SINGLETON_MANAGER_MEMORY_MANAGER_H
