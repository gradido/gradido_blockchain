#ifndef __GRADIDO_BLOCKCHAIN_DATA_HIERO_TOPIC_ID_H
#define __GRADIDO_BLOCKCHAIN_DATA_HIERO_TOPIC_ID_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

namespace hiero
{
	class GRADIDOBLOCKCHAIN_EXPORT TopicId
	{
	public:
		TopicId();
		TopicId(int64_t shardNum, int64_t realmNum, int64_t topicNum);
		// string like 0.0.241
		TopicId(const std::string& topicIdString);
		~TopicId();

		// default constructor and operators
		TopicId(const TopicId& other) = default;
		TopicId(TopicId&& other) noexcept = default;
		TopicId& operator=(const TopicId& other) = default;
		TopicId& operator=(TopicId&& other) noexcept = default;

		int64_t getShardNum() const { return mShardNum; }
		int64_t getRealmNum() const { return mRealmNum; }
		int64_t getTopicNum() const { return mTopicNum; }

		std::string toString() const;
		inline bool operator==(const TopicId& other) const;

		inline bool empty() const { return !mTopicNum; }

	protected:
		int64_t mShardNum;
		int64_t mRealmNum; 
		int64_t mTopicNum;
	};

	inline bool TopicId::operator==(const TopicId& other) const
	{
		return mShardNum == other.mShardNum
			&& mRealmNum == other.mRealmNum
			&& mTopicNum == other.mTopicNum
		;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_HIERO_TOPIC_ID_H