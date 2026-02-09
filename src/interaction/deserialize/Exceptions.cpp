#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

#include <string>

using std::string;

namespace gradido::interaction::deserialize {	
	string MissingMemberException::getFullString() const
	{
		string result(what());
		result += ", member name: " + mMemberName;
		return result;
	}

	string InvalidMemberException::getFullString() const
	{
		string result(what());
		result += ", member name: " + mMemberName;
		result += ", member value " + mMemberValue;
		result += ", expected value: " + mExpectedValue;
		return result;
	}

	string EmptyMemberException::getFullString() const 
	{
		string result(what());
		result += ", member name: " + mMemberName;
		return result;
	}
}