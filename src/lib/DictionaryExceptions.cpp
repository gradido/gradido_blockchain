#include "gradido_blockchain/lib/DictionaryExceptions.h"

std::string DictionaryOverflowException::getFullString() const 
{ 
    std::string result = what();
    result += ",    Dictionary name: " + mDictionaryName;
	return result;  
}