#include "gtest/gtest.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

using namespace memory;


TEST(BlockTest, Constructor) {
	// Test constructor with size only
	Block block(10);
	EXPECT_EQ(block.size(), 10);

	// Test constructor with data and size
	unsigned char data[] = { 0x01, 0x02, 0x03 };
	Block block2(3, data);
	EXPECT_EQ(block2.size(), 3);
	EXPECT_EQ(block2.data()[0], 0x01);
	EXPECT_EQ(block2.data()[1], 0x02);
	EXPECT_EQ(block2.data()[2], 0x03);

	// Test constructor with vector of unsigned chars
	std::vector<unsigned char> vec = { 0x04, 0x05, 0x06 };
	Block block3(vec);
	EXPECT_EQ(block3.size(), vec.size());
	for (size_t i = 0; i < vec.size(); ++i) {
		EXPECT_EQ(block3.data()[i], vec[i]);
	}

	// Test constructor with std::span
	std::byte dataBytes[] = { std::byte{0x07}, std::byte{0x08}, std::byte{0x09} };
	std::span<std::byte> span(dataBytes, 3);
	Block block4(span);
	EXPECT_EQ(block4.size(), 3);
	for (size_t i = 0; i < 3; ++i) {
		EXPECT_EQ(block4.data()[i], std::to_integer<int>(dataBytes[i]));
	}

	// Test constructor with std::string
	std::string str = "0123456789";
	Block block5(str);
	EXPECT_EQ(block5.size(), str.size());
	for (size_t i = 0; i < str.size(); ++i) {
		EXPECT_EQ(block5.data()[i], static_cast<unsigned char>(str[i]));
	}
}

TEST(BlockTest, Comparisation) {
	unsigned char data[]  = { 0x01, 0x02, 0x03 };
	unsigned char data2[] = { 0x02, 0x12, 0x04 };
	unsigned char data3[] = { 0x01, 0x02, 0x03, 0x04 };
	Block a(3, data);
	Block b(3, data2);
	Block c(4, data3);
	Block d(3, data3);
	EXPECT_TRUE(a.isTheSame(d));
	EXPECT_FALSE(a.isTheSame(b));
	EXPECT_FALSE(a.isTheSame(c));
	EXPECT_FALSE(b.isTheSame(c));
	EXPECT_TRUE(d.isTheSame(a));
}

TEST(BlockTest, AssignmentOperators) {
	unsigned char data[]  = { 0x01, 0x02, 0x03 };
	unsigned char data2[] = { 0x01, 0x02, 0x03, 0x04 };

	// Test copy assignment
	Block block1(3, data);
	Block block2 = block1;
	EXPECT_EQ(block2.size(), 3);
	EXPECT_TRUE(block1.isTheSame(block2));

	// Test move assignment
	Block block3(4, data2);
	Block block4 = std::move(block3);
	EXPECT_EQ(block4.size(), 4);
	EXPECT_FALSE(block3.size());
	EXPECT_FALSE(block3.data());
	EXPECT_FALSE(block3.isTheSame(block4));	

	// Test copy constructor
	Block block5(30);
	Block block6(block5);
	EXPECT_EQ(block6.size(), 30);

	// Test move constructor
	Block block7 = std::move(block5);
	EXPECT_EQ(block7.size(), 30);
	EXPECT_FALSE(block5.isTheSame(block7));
}


TEST(BlockTest, Size) {
	// Test size method
	Block block(20);
	EXPECT_EQ(block.size(), 20);

	// Test size method on copy constructor created block
	Block block2 = block;
	EXPECT_EQ(block2.size(), 20);
}

TEST(BlockTest, Data) {
	// Test data method
	unsigned char data[] = { 0x01, 0x02, 0x03 };
	Block block(3, data);
	EXPECT_EQ(block.data()[0], 0x01);
	EXPECT_EQ(block.data()[1], 0x02);
	EXPECT_EQ(block.data()[2], 0x03);

	// Test data method on copy constructor created block
	Block block2 = block;
	EXPECT_EQ(block2.data()[0], 0x01);
	EXPECT_EQ(block2.data()[1], 0x02);
	EXPECT_EQ(block2.data()[2], 0x03);
}

TEST(BlockTest, ConvertToAndFromHex) {
	Block block(18);
	constexpr auto resultHex = "000102030405060708090a0b0c0d0e0f1011";
	// Initialize block data
	for (size_t i = 0; i < block.size(); ++i) {
		block.data()[i] = static_cast<unsigned char>(i % 256);
	}
	EXPECT_EQ(block.convertToHex(), resultHex);

	Block hexBlock = Block::fromHex(resultHex, strlen(resultHex));
	EXPECT_EQ(hexBlock, block);

	// invalid hex
	EXPECT_THROW(Block::fromHex("000102030405060708090a0b0c0d0e0f101", 35), GradidoInvalidHexException);
	// invalid hex
	EXPECT_THROW(Block::fromHex("abhz", 5), GradidoInvalidHexException);
}

TEST(BlockTest, ConvertToAndFromBase64) {
	Block block(10);
	constexpr auto resultBase64 = "AAECAwQFBgcICQ==";
	// Initialize block data
	for (size_t i = 0; i < block.size(); ++i) {
		block.data()[i] = static_cast<unsigned char>(i % 256);
	}
	EXPECT_EQ(block.convertToBase64(), resultBase64);
	Block base64Block = Block::fromBase64(resultBase64);
	EXPECT_EQ(block, base64Block);

	// invalid base64
	EXPECT_THROW(Block::fromBase64("AAECAwQFBgcICQ"), GradidoInvalidBase64Exception);
	try {
		Block::fromBase64("AAECAwQFBgc");
		printf("valid\n");
	} catch(GradidoInvalidBase64Exception& ex) {
		printf("invalid base64: %s\n", ex.getFullString().data());
	}
}

TEST(BlockTest, Blake2bHash) {
	Block block(10);
	// Initialize block data
	for (size_t i = 0; i < block.size(); ++i) {
		block.data()[i] = static_cast<unsigned char>(i % 256);
	}
	auto hash = block.calculateHash();
	EXPECT_EQ(hash.convertToHex(), "8b57a796a5d07cb04cc1614dfc2acb3f73edc712d7f433619ca3bbe66bb15f49");
}

TEST(BlockTest, isEmpty) {
	Block block(10);
	// Initialize block data
	for (size_t i = 0; i < block.size(); ++i) {
		block.data()[i] = static_cast<unsigned char>(i % 256);
	}
	Block emptyBlock(10);
	EXPECT_TRUE(emptyBlock.isEmpty());
	EXPECT_FALSE(block.isEmpty());
	EXPECT_EQ(emptyBlock.convertToHex(), "00000000000000000000");
}

TEST(BlockTest, CopyAsString) {
	Block block(10);
	// Initialize block data
	for (size_t i = 0; i < block.size(); ++i) {
		block.data()[i] = static_cast<unsigned char>(i % 256);
	}
	std::string copyStr = block.copyAsString();
	EXPECT_EQ(copyStr.size(), block.size());
	for (size_t i = 0; i < block.size(); ++i) {
		EXPECT_EQ(copyStr[i], static_cast<char>(block.data()[i]));
	}
}

TEST(BlockTest, CopyAsVector) {
	Block block(10);
	// Initialize block data
	for (size_t i = 0; i < block.size(); ++i) {
		block.data()[i] = static_cast<unsigned char>(i % 256);
	}
	auto copyVec = block.copyAsVector();
	EXPECT_EQ(copyVec.size(), block.size());
	for (size_t i = 0; i < block.size(); ++i) {
		EXPECT_EQ(copyVec[i], static_cast<char>(block.data()[i]));
	}
}
