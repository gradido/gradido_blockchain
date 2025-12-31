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

DictionaryInsertException::DictionaryInsertException(
	const char* what,
	const char* dictionaryName,
	const char* key,
	const char* data
) noexcept : DictionaryException(what, dictionaryName), mKey(key), mData(data)
{

}

std::string DictionaryInsertException::getFullString() const
{
	std::string result = what();
	result += ", dictionary name: " + mDictionaryName;
	result += ", key: " + mKey;
	result += ", data: " + mData;
	return result;
}

DictionaryInvalidNewKeyException::DictionaryInvalidNewKeyException(
	const char* what, 
	const char* dictionaryName,
	const char* newKeyString,
	const char* oldKeyString
) noexcept : DictionaryException(what, dictionaryName), mNewKeyString(newKeyString), mOldKeyString(oldKeyString)
{

}

std::string DictionaryInvalidNewKeyException::getFullString() const
{
	std::string result = what();
	result += ", dictionary name: " + mDictionaryName;
	result += ", current key: " + mOldKeyString;
	result += ", new key: " + mNewKeyString;
	return result;
}