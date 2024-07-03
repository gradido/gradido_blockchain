#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

#include "gradido_blockchain/interaction/serialize/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {
			std::string MissingMemberException::getFullString() const
			{
				std::string result(what());
				result += ", member name: " + mMemberName;
				return result;
			}
		}
	}
}