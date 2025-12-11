#include "gradido_blockchain/data/hiero/TopicId.h"
#include "gtest/gtest.h"

using namespace hiero;

TEST(HieroTopicIdTest, Constructor) 
{
	auto topicId = TopicId(0, 0, 17);
	EXPECT_EQ(topicId.getShardNum(), 0);
	EXPECT_EQ(topicId.getRealmNum(), 0);
	EXPECT_EQ(topicId.getTopicNum(), 17);
	
	topicId = TopicId("0.-121.18");
	EXPECT_EQ(topicId.getShardNum(), 0);
	EXPECT_EQ(topicId.getRealmNum(), -121);
	EXPECT_EQ(topicId.getTopicNum(), 18);
}

TEST(HieroTopicIdTest, toString)
{
	auto topicId = TopicId(0, 0, 2718);
	EXPECT_EQ(topicId.toString(), "0.0.2718");

	topicId = TopicId(0, -2, 2819);
	EXPECT_EQ(topicId.toString(), "0.-2.2819");
}


