#include "gradido_blockchain/v3_3/blockchain/AbstractProvider.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			std::string GroupNotFoundException::getFullString() const
			{
				std::string result = what();
				result += ", community id: " + mCommunityId;
				return result;
			}
		}
	} 
}