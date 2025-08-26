#include "gradido_blockchain/data/hiero/TopicId.h"
#include <loguru.hpp>

namespace hiero {
	TopicId::TopicId()
		: mShardNum(0), mRealmNum(0), mTopicNum(0)
	{

	}
	TopicId::TopicId(int64_t shardNum, int64_t realmNum, int64_t topicNum)
		: mShardNum(shardNum), mRealmNum(realmNum), mTopicNum(topicNum)
	{

	}

	TopicId::TopicId(const std::string& topicIdString)
		: mShardNum(0), mRealmNum(0), mTopicNum(0)
	{
		char* end;
		const char* str = topicIdString.c_str();
		mShardNum = strtoll(str, &end, 10);
		if (end - str >= topicIdString.size()) {
			LOG_F(ERROR, "string don't seem to contain enough numbers");
			return;
		}
		if (*end != '.') LOG_F(WARNING, "expect . after first number, but get: %c", *end);
		mRealmNum = strtoll(end + 1, &end, 10);
		if (end - str >= topicIdString.size()) {
			LOG_F(ERROR, "string don't seem to contain enough numbers");
			return;
		}
		if (*end != '.') LOG_F(WARNING, "expect . after second number, but get: %c", *end);
		mTopicNum = strtoll(end + 1, &end, 10);
	}

	TopicId::~TopicId()
	{

	}

	std::string TopicId::toString() const
	{
		std::string shardNumStr = "0";
		std::string realmNumStr = "0";
		if (mShardNum) {
			shardNumStr = std::to_string(mShardNum);
		}
		if (mRealmNum) {
			realmNumStr = std::to_string(mRealmNum);
		}
		
		std::string accountNumStr = std::to_string(mTopicNum);
		std::string result;
		result.reserve(2 + shardNumStr.size() + realmNumStr.size() + accountNumStr.size());
		result = shardNumStr + '.' + realmNumStr + '.' + accountNumStr;
		return result;
	}
}