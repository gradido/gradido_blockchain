#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <string_view>

class GRADIDOBLOCKCHAIN_EXPORT DictionaryException : public GradidoBlockchainException
{
public:
	explicit DictionaryException(const char* what) noexcept 
		: GradidoBlockchainException(what) {}
};

class GRADIDOBLOCKCHAIN_EXPORT DictionaryOverflowException : public DictionaryException
{
public:
	explicit DictionaryOverflowException(const char* what, std::string_view dictionaryName) noexcept
		: DictionaryException(what), mDictionaryName(dictionaryName) {}

	std::string getFullString() const;

protected:
	std::string mDictionaryName;
};

class GRADIDOBLOCKCHAIN_EXPORT DictionaryMissingEntryException : public DictionaryException
{
public:
	explicit DictionaryMissingEntryException(const char* what, std::string_view missingEntryValue) noexcept
		: DictionaryException(what), mMissingEntryValue(missingEntryValue) {
	}

	std::string getFullString() const;

protected:
	std::string mMissingEntryValue;
};

class GRADIDOBLOCKCHAIN_EXPORT DictionaryHoleException : public DictionaryException
{
public:
	explicit DictionaryHoleException(const char* what, const char* name, size_t currentSize, size_t newIndex) noexcept
		: DictionaryException(what), mName(name), mCurrentSize(currentSize), mNewIndex(newIndex) { }

	std::string getFullString() const;

protected:
	std::string mName;
	size_t mCurrentSize;
	size_t mNewIndex;
};


#endif // __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H