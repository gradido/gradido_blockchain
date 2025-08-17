#include "gradido_blockchain/data/hiero/AccountId.h"
#include <loguru.hpp>

namespace hiero {

	std::string AccountId::toString()
	{
		std::string shardNumStr = "0";
		std::string realmNumStr = "0";
		if (mShardNum) {
			shardNumStr = std::to_string(mShardNum);
		}
		if (mRealmNum) {
			realmNumStr = std::to_string(mRealmNum);
		}
		if (!mAlias.isEmpty()) {
			auto aliasHex = mAlias.convertToHex();
			LOG_F(ERROR, "alias isn't empty, this case is currently not implemented, return hex version %s", aliasHex.data());
			std::string result; 
			result.reserve(2 + shardNumStr.size() + realmNumStr.size() + aliasHex.size());
			result = shardNumStr + '.' + realmNumStr + '.' + aliasHex;
			return result;
		}
		else {
			std::string accountNumStr = std::to_string(mAccountNum);
			std::string result;
			result.reserve(2 + shardNumStr.size() + realmNumStr.size() + accountNumStr.size());
			result = shardNumStr + '.' + realmNumStr + '.' + accountNumStr;
			return result;
		}

	}
}