#include "gradido_blockchain/blockchain/AbstractProvider.h"

namespace gradido {
	namespace blockchain {

		AbstractProvider::AbstractProvider()
			: mCoinCommunityIdDicitionary("CoinCommunityId")
		{

		}

		std::string GroupNotFoundException::getFullString() const
		{
			std::string result = what();
			result += ", community id: " + mCommunityId;
			return result;
		}
	} 
}