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

class DictionaryOverflowException : public DictionaryException
{
public:
	explicit DictionaryOverflowException(const char* what, std::string_view dictionaryName) noexcept
		: DictionaryException(what), mDictionaryName(dictionaryName) {}

	std::string getFullString() const;

protected:
	std::string mDictionaryName;
};

#endif // __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_EXCEPTIONS_H