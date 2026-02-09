#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include <string>

using std::string, std::to_string;


string DictionaryOverflowException::getFullString() const 
{ 
    string result = what();
    result += ",    Dictionary name: " + mDictionaryName;
	return result;  
}

string DictionaryMissingEntryException::getFullString() const
{
    string result = what();
    result += ", missing entry: " + mMissingEntryValue;
    return result;
}

string DictionaryHoleException::getFullString() const
{
  string result = what();
  result += ", in dictionary: " + mName;
  result += ", current size: " + to_string(mCurrentSize);
  result += ", new index size: " + to_string(mNewIndex);

  return result;
}