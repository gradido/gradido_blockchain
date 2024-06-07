#include "gtest/gtest.h"
#include "../../KeyPairs.h"
#include "gradido_blockchain/v3_3/interaction/serialize/Context.h"
#include "gradido_blockchain/v3_3/TransactionBodyBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

using namespace gradido::v3_3;
using namespace data;
using namespace interaction;

#define VERSION_STRING "3.3"
const auto createdAt = std::chrono::system_clock::from_time_t(1609459200); //2021-01-01 00:00:00 UTC

TEST(SerializeTest, TransactionBodyWithoutMemo)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}


TEST(SerializeTest, TransactionBody)
{
	TransactionBody body("memo", createdAt, VERSION_STRING);
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}

TEST(SerializeTest, CommunityRootBody) 
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	EXPECT_TRUE(body.isCommunityRoot());
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}

TEST(SerializeTest, RegisterAddressBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		g_KeyPairs[3].publicKey,
		AddressType::COMMUNITY_HUMAN,
		1,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	EXPECT_TRUE(body.isRegisterAddress());
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}

TEST(SerializeTest, GradidoCreationBody) {
	TransactionBody body("Deine erste Sch�pfung ;)", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
		TimestampSeconds(1609459000)
	);
	EXPECT_TRUE(body.isCreation());
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}

TEST(SerializeTest, GradidoTransferBody) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[5].publicKey
	);
	EXPECT_TRUE(body.isTransfer());
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}

TEST(SerializeTest, GradidoDeferredTransferBody) {
	TransactionBody body("Link zum einloesen", createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "555.55"),
			g_KeyPairs[5].publicKey
		), TimestampSeconds(1609465000)
	);
	EXPECT_TRUE(body.isDeferredTransfer());
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}

TEST(SerializeTest, CommunityFriendsUpdateBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityFriendsUpdate = make_shared<CommunityFriendsUpdate>(true);
	EXPECT_TRUE(body.isCommunityFriendsUpdate());
	serialize::Context c(body);
	auto serialized = c.run();
	printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
}