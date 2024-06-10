#include "gradido_blockchain/v3_3/interaction/deserialize/Exceptions.h"

#include "gradido_blockchain/v3_3/interaction/serialize/Exceptions.h"

namespace gradido {
	namespace v3_3 {
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
}