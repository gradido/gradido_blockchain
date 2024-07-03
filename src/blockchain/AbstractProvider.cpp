#include "gradido_blockchain/blockchain/AbstractProvider.h"

namespace gradido {
	namespace blockchain {
		std::string GroupNotFoundException::getFullString() const
		{
			std::string result = what();
			result += ", community id: " + mCommunityId;
			return result;
		}
	} 
}