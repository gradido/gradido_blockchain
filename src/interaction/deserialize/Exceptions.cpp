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

	string EmptyMemberException::getFullString() const 
	{
		string result(what());
		result += ", member name: " + mMemberName;
		return result;
	}
}