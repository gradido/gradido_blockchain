#include "gradido_blockchain/lib/DictionaryExceptions.h"

std::string DictionaryOverflowException::getFullString() const 
{ 
    std::string result = what();
    result += ",    Dictionary name: " + mDictionaryName;
	return result;  
}

std::string DictionaryMissingEntryException::getFullString() const
{
    std::string result = what();
    result += ", missing entry: " + mMissingEntryValue;
    return result;
}