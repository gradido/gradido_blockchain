//#include "DataTypeConverter.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include <gtest/gtest.h>

#include <chrono>
using namespace std;
using namespace chrono;
using namespace DataTypeConverter;

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

TEST(DataTypeConverterTest, TimePointToString)
{
	auto firstAppleReleaseDate = system_clock::time_point(seconds{ 443770227 });
	ASSERT_EQ(timePointToString(firstAppleReleaseDate, "%Y-%m-%d %H:%M:%S"), "1984-01-24 05:30:27.0000000");
}

TEST(DataTypeConverterTest, DateTimeStringToTimePoint)
{
	std::string firstIBMRelease = "1981-08-12 06:30:27";
	auto timepoint = dateTimeStringToTimePoint(firstIBMRelease, "%F %T");
	ASSERT_EQ(duration_cast<seconds>(timepoint.time_since_epoch()).count(), 366445827);
}