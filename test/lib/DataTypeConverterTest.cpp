//#include "DataTypeConverter.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include <gtest/gtest.h>

std::string g_Base64_Examples[] = {
	"RWluZSBMaWNodHVuZyBpbSBXYWxkLg=="
};

std::string g_Base64_Example_Bins[] = {
	"Eine Lichtung im Wald."
};

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}


TEST(DataTypeConverterTest, Base64ToBinString)
{
	ASSERT_EQ(DataTypeConverter::base64ToBinString(g_Base64_Examples[0]), g_Base64_Example_Bins[0]);
}

TEST(DataTypeConverterTest, BinToBase64)
{
	ASSERT_EQ(DataTypeConverter::binToBase64(g_Base64_Example_Bins[0]), g_Base64_Examples[0]);
}

TEST(DataTypeConverterTest, BinToBase64UniqueString)
{
	std::unique_ptr<std::string> binUniqueString(new std::string(g_Base64_Example_Bins[0]));
	auto base64 = DataTypeConverter::binToBase64(std::move(binUniqueString));
	ASSERT_EQ(base64->data(), g_Base64_Examples[0]);
}
