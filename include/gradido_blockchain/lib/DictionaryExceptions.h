#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"

#include <string_view>

class DictionaryException : public GradidoBlockchainException
{
public:
	explicit DictionaryException(const char* what) noexcept 
		: GradidoBlockchainException(what) {}
};

class DictionaryOverflowException : public DictionaryException
{
public:
	explicit DictionaryOverflowException(const char* what) noexcept
		: DictionaryException(what) {}

	std::string getFullString() const;
	inline void setDictionaryName(std::string_view dictionaryName) {
		mDictionaryName = dictionaryName;
	}

protected:
	std::string mDictionaryName;
};

#endif // __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H