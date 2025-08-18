#include "gradido_blockchain/data/hiero/AccountId.h"
#include <loguru.hpp>

using namespace std;

namespace hiero {
	AccountId::AccountId()
		: mShardNum(0), mRealmNum(0), mAccountNum(0), mAlias(0)
	{

	}

	AccountId::AccountId(int64_t shardNum, int64_t realmNum, int64_t accountNum)
		: mShardNum(shardNum), mRealmNum(realmNum), mAccountNum(accountNum), mAlias(memory::Block(0))
	{

	}

	AccountId::AccountId(int64_t shardNum, int64_t realmNum, const memory::Block& alias)
		: mShardNum(shardNum), mRealmNum(realmNum), mAccountNum(0), mAlias(alias)
	{

	}

	AccountId::AccountId(int64_t shardNum, int64_t realmNum, memory::Block&& alias)
		: mShardNum(shardNum), mRealmNum(realmNum), mAccountNum(0), mAlias(std::move(alias))
	{

	}

	AccountId::~AccountId()
	{

	}
	
	AccountId::AccountId(const std::string& accountIdString)
		: mShardNum(0), mRealmNum(0), mAccountNum(0), mAlias(0)
	{
		char* end;
		const char* str = accountIdString.c_str();
		mShardNum = strtoll(str, &end, 10);
		if (end - str >= accountIdString.size()) {
			LOG_F(ERROR, "string don't seem to contain enough numbers");
			return; 
		}
		if (*end != '.') LOG_F(WARNING, "expect . after first number, but get: %c", *end);
		mRealmNum = strtoll(end + 1, &end, 10);
		if (end - str >= accountIdString.size()) {
			LOG_F(ERROR, "string don't seem to contain enough numbers");
			return;
		}
		if (*end != '.') LOG_F(WARNING, "expect . after second number, but get: %c", *end);
		mAccountNum = strtoll(end + 1, &end, 10);
	}

	std::string AccountId::toString() const
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