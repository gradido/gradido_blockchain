#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H

#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <unordered_map>
#include <shared_mutex>
#include <memory>

/*!
* @author Dario Rekowski
* @date 2020-02-06
*
* @brief Create Indices for strings, for more efficient working with strings in memory
* So basically every one use only the uint32_t index value for comparisation and stuff and only if the real value is needed,
* they will ask the Dictionary for the real value
*
*/

namespace memory {
	class Block;
	using ConstBlockPtr = std::shared_ptr<const Block>;
}

class Dictionary
{
public:
	Dictionary(const char* name);
	virtual ~Dictionary();
	
	//! empty 
	virtual void reset();

	virtual uint32_t getIndexForBinary(memory::ConstBlockPtr binary) const;
	virtual inline bool hasBinary(memory::ConstBlockPtr binary) const {
		return findIndexForBinary(binary) != 0;
	}

	virtual memory::ConstBlockPtr getBinaryForIndex(uint32_t index) const;		
	virtual bool hasIndex(uint32_t index) const;

	//! \brief Get or add index if not exist in cache or file, maybe I/O read.
	//! \param address User public key as binary string
	//! \param lastIndex Last knowing index for group.
	//! \return Index for address.
	virtual uint32_t getOrAddIndexForBinary(memory::ConstBlockPtr binary);

	//! \brief Add index
	//! \return False if index address already exist, else true.
	virtual bool addBinaryIndex(memory::ConstBlockPtr binary, uint32_t index);

	virtual inline uint32_t getLastIndex() { return mLastIndex; }

protected:
	bool hasBinaryIndexPair(memory::ConstBlockPtr binary, uint32_t index) const;
	uint32_t findIndexForBinary(memory::ConstBlockPtr binary) const;

	typedef std::pair<memory::ConstBlockPtr, uint32_t> BinaryIndexPair;

	std::string mName;
	uint32_t mLastIndex;	
	// for finding index for a specific binary data blob
	std::unordered_multimap<SignatureOctet, BinaryIndexPair> mBinaryIndexLookup;
	// for finding binary data blob for a specific index
	std::unordered_map<uint32_t, memory::ConstBlockPtr> mIndexBinaryLookup;
};

class DictionaryThreadsafe : protected Dictionary
{
public:
	using Dictionary::Dictionary;

	virtual void reset() override;

	virtual uint32_t getIndexForBinary(memory::ConstBlockPtr binary) const override;

	virtual memory::ConstBlockPtr getBinaryForIndex(uint32_t index) const override;
	virtual bool hasIndex(uint32_t index) const override;

	//! \brief Get or add index if not exist in cache or file, maybe I/O read.
	//! \param address User public key as binary string
	//! \param lastIndex Last knowing index for group.
	//! \return Index for address.
	virtual uint32_t getOrAddIndexForBinary(memory::ConstBlockPtr binary) override;

	//! \brief Add index
	//! \return False if index address already exist, else true.
	virtual bool addBinaryIndex(memory::ConstBlockPtr binary, uint32_t index) override;

	virtual inline uint32_t getLastIndex() override { std::shared_lock _lock(mSharedMutex);  return mLastIndex; }
protected:
	mutable std::shared_mutex mSharedMutex;
};


class DictionaryException : public GradidoBlockchainException
{
public:
	explicit DictionaryException(const char* what, const char* dictionaryName) noexcept;
protected:
	std::string mDictionaryName;
};

class DictionaryNotFoundException : public DictionaryException
{
public:
	explicit DictionaryNotFoundException(const char* what, const char* dictionaryName, const char* key) noexcept;
	std::string getFullString() const;

protected:
	std::string mKey;
};

class DictionaryInvalidNewKeyException : public DictionaryException
{
public:
	explicit DictionaryInvalidNewKeyException(const char* what, const char* dictionaryName, uint32_t newKey, uint32_t oldKey) noexcept;
	std::string getFullString() const;

protected:
	uint32_t mNewKey;
	uint32_t mOldKey;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
