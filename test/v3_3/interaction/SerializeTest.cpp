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
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized), "CgASCAiAzLn/BRAAGgMzLjMgAA==");
}


TEST(SerializeTest, TransactionBody)
{
	TransactionBody body("memo", createdAt, VERSION_STRING);
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized), "CgRtZW1vEggIgMy5/wUQABoDMy4zIAA=");
}

TEST(SerializeTest, CommunityRootBody) 
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized), 
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hogUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	);
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
	ASSERT_TRUE(body.isRegisterAddress());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized),
		"CgASCAiAzLn/BRAAGgMzLjMgAEpICiAllxqg50IhRNzCRIh+Ke8WDVR5sSGemBfKbs44sJ83wBABIiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VygB"
	);

}

TEST(SerializeTest, GradidoCreationBody) {
	TransactionBody body("Deine erste Schoepfung ;)", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized),
		"ChlEZWluZSBlcnN0ZSBTY2hvZXBmdW5nIDspEggIgMy5/wUQABoDMy4zIAA6TgpECiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VxIeMTAwMC4wMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwGgAaBgi4yrn/BQ=="
	);
}

TEST(SerializeTest, GradidoTransferBody) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized),
		"ChFJY2ggdGVpbGUgbWl0IGRpchIICIDMuf8FEAAaAzMuMyAAMmcKQwogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSHTUwMC41NTAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwGgASINGpWCTISFkAJ5uSpgF1/GdviRTGHXOZxmwtDLb6nsV2"
	);
}

TEST(SerializeTest, GradidoDeferredTransferBody) {
	TransactionBody body("Link zum einloesen", createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "555.55"),
			g_KeyPairs[5].publicKey
		), TimestampSeconds(1609465000)
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized),
		"ChJMaW5rIHp1bSBlaW5sb2VzZW4SCAiAzLn/BRAAGgMzLjMgAFJxCmcKQwogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSHTU1NS41NTAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwGgASINGpWCTISFkAJ5uSpgF1/GdviRTGHXOZxmwtDLb6nsV2EgYIqPm5/wU="
	);
}

TEST(SerializeTest, CommunityFriendsUpdateBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityFriendsUpdate = make_shared<CommunityFriendsUpdate>(true);
	ASSERT_TRUE(body.isCommunityFriendsUpdate());
	serialize::Context c(body);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized),
		"CgASCAiAzLn/BRAAGgMzLjMgAEICCAE="
	);
}

TEST(SerializeTest, GradidoTransaction) {
	GradidoTransaction transaction;
	auto bodyBytes = make_shared<memory::Block>(
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson "
	);
	auto sign = make_shared<memory::Block>(64);
	auto& keyPair = g_KeyPairs[3];
	unsigned long long actualSignLength = 0;
	crypto_sign_detached(*sign, &actualSignLength, *bodyBytes, bodyBytes->size(), *keyPair.privateKey);
	transaction.mSignatureMap.push({ keyPair.publicKey, keyPair.privateKey });
	transaction.mBodyBytes = bodyBytes;
	
	serialize::Context c(transaction);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(DataTypeConverter::binToBase64(*serialized),
		"CmYKZAogJZcaoOdCIUTcwkSIfinvFg1UebEhnpgXym7OOLCfN8ASQNYLm6Z41YR3uoWtehLSswGDM8p0XropF8u+Emh+txDwJZcaoOdCIUTcwkSIfinvFg1UebEhnpgXym7OOLCfN8ASigEnJ1RvIGJlIHlvdXJzZWxmIGluIGEgd29ybGQgdGhhdCBpcyBjb25zdGFudGx5IHRyeWluZyB0byBtYWtlIHlvdSBzb21ldGhpbmcgZWxzZSBpcyB0aGUgZ3JlYXRlc3QgYWNjb21wbGlzaG1lbnQuJycKIC0gUmFscGggV2FsZG8gRW1lcnNvbiA="
	);
}