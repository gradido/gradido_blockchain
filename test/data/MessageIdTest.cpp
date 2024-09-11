#include "gradido_blockchain/data/iota/MessageId.h"
#include "gtest/gtest.h"

using namespace iota;
using namespace memory;

const auto testHex1 = "4f1a5e6b7c9d3a2e0f8b7d4c1e9f5a6b3c7d8e9f0a1b2c3d4e5f6a7b8c9d0e1f";
const auto testHex2 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

TEST(MessageIdTest, Constructor)
{
	Block b(Block::fromHex(testHex1));
	MessageId idFromHex(testHex1);
	MessageId idFromBlock(b);

	EXPECT_EQ(idFromHex.getMessageIdByte(0), 0x2e3a9d7c6b5e1a4f);
	EXPECT_EQ(idFromHex.getMessageIdByte(1), 0x6b5a9f1e4c7d8b0f);
	EXPECT_EQ(idFromHex.getMessageIdByte(2), 0x3d2c1b0a9f8e7d3c);
	EXPECT_EQ(idFromHex.getMessageIdByte(3), 0x1f0e9d8c7b6a5f4e);

	EXPECT_EQ(idFromBlock.getMessageIdByte(0), 0x2e3a9d7c6b5e1a4f);
	EXPECT_EQ(idFromBlock.getMessageIdByte(1), 0x6b5a9f1e4c7d8b0f);
	EXPECT_EQ(idFromBlock.getMessageIdByte(2), 0x3d2c1b0a9f8e7d3c);
	EXPECT_EQ(idFromBlock.getMessageIdByte(3), 0x1f0e9d8c7b6a5f4e);
}

TEST(MessageIdTest, Operators)
{
	MessageId id1(testHex1);
	MessageId id2(testHex2);
	// test operator ==
	EXPECT_NE(id1, id2);
	EXPECT_EQ(id1, id1);
	EXPECT_EQ(id2, id2);

	// test operator <
	EXPECT_LT(id2, id1);

	// test operator std::string
	EXPECT_EQ(id1, std::string(testHex1));
	EXPECT_EQ(id2, std::string(testHex2));
	EXPECT_NE(id1, std::string(testHex2));
	EXPECT_NE(id2, std::string(testHex1));
}

TEST(MessageIdTest, Empty)
{
	MessageId emptyId;
	MessageId id(testHex1);
	EXPECT_TRUE(emptyId.isEmpty());
	EXPECT_FALSE(id.isEmpty());
}

TEST(MessageIdTest, toMemoryBlock)
{
	MessageId id(testHex1);
	auto block = id.toMemoryBlock();
	EXPECT_EQ(block.convertToHex(), testHex1);
}