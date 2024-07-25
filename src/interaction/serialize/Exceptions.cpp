#include "gradido_blockchain/interaction/serialize/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace serialize {
			std::string MissingMemberException::getFullString() const
			{
				std::string result(what());
				result += ", member name: " + mMemberName;
				return result;
			}
		}
	}
}