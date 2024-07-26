#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "const.h"
#include "gradido_blockchain/interaction/toJson/Context.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum_flags.hpp"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ToJsonTest, TransactionBodyWithoutMemo)
{
	TransactionBody body("", createdAt, VERSION_STRING);

	toJson::Context c(body);
	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\"}");

	auto jsonPretty = c.run(true);
	//printf("json pretty: %s\n", jsonPretty.data());
	EXPECT_EQ(jsonPretty, "{\n    \"memo\": \"\",\n    \"createdAt\": \"2021-01-01 00:00:00.0000\",\n    \"versionNumber\": \"3.3\",\n    \"type\": \"LOCAL\"\n}");	

}


TEST(ToJsonTest, TransactionBody)
{
	TransactionBody body("memo", createdAt, VERSION_STRING);

	toJson::Context c(body);
	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"memo\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\"}");
	
}

TEST(ToJsonTest, CommunityRootBody)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());

	toJson::Context c(body);
	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"communityRoot\":{\"pubkey\":\"643c438776fc2634faf887df8485b9ed580729c2099e00e4d4d53cd74626a0d6\",\"gmwPubkey\":\"51f9b1e8d984763add4d90cc6422f1fd4a09c677b98e4b198c055bc29420f58e\",\"aufPubkey\":\"bb994a1d62e7b3ca7b9f3a7e92a8ed6112a53f76c49595b517cc7450a50b7a5a\"}}");
}

TEST(ToJsonTest, RegisterAddressBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());

	toJson::Context c(body);
	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"registerAddress\":{\"userPubkey\":\"25971aa0e7422144dcc244887e29ef160d5479b1219e9817ca6ece38b09f37c0\",\"addressType\":\"COMMUNITY_HUMAN\",\"accountPubkey\":\"8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57\",\"derivationIndex\":1}}");
}

TEST(ToJsonTest, GradidoCreationBody) {
	TransactionBody body("Deine erste Schoepfung ;)", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());

	toJson::Context c(body);
	auto json = c.run();
	// printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"Deine erste Schoepfung ;)\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"creation\":{\"recipient\":{\"pubkey\":\"8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57\",\"amount\":\"1000.0000\"},\"targetDate\":\"2020-12-31 23:56:40.0000\"}}");
}

TEST(ToJsonTest, GradidoTransferBody) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	toJson::Context c(body);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"Ich teile mit dir\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57\",\"amount\":\"500.5500\"},\"recipient\":\"d1a95824c8485900279b92a60175fc676f8914c61d7399c66c2d0cb6fa9ec576\"}}");
}

TEST(ToJsonTest, GradidoDeferredTransferBody) {
	TransactionBody body("Link zum einloesen", createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "555.55"),
			g_KeyPairs[5].publicKey
		), TimestampSeconds(1609465000)
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	toJson::Context c(body);

	auto json = c.run();
	// printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"Link zum einloesen\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"deferredTransfer\":{\"transfer\":{\"sender\":{\"pubkey\":\"8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57\",\"amount\":\"555.5500\"},\"recipient\":\"d1a95824c8485900279b92a60175fc676f8914c61d7399c66c2d0cb6fa9ec576\"},\"timeout\":\"2021-01-01 01:36:40.0000\"}}");
}

TEST(ToJsonTest, CommunityFriendsUpdateBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityFriendsUpdate = make_shared<CommunityFriendsUpdate>(true);
	ASSERT_TRUE(body.isCommunityFriendsUpdate());
	toJson::Context c(body);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"communityFriendsUpdate\":{\"colorFusion\":true}}");
}

TEST(ToJsonTest, GradidoTransaction) {
	GradidoTransaction transaction;
	auto bodyBytes = make_shared<memory::Block>(
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson "
	);
	auto sign = make_shared<memory::Block>(crypto_sign_BYTES);
	auto& keyPair = g_KeyPairs[3];
	unsigned long long actualSignLength = 0;
	crypto_sign_detached(*sign, &actualSignLength, *bodyBytes, bodyBytes->size(), *keyPair.privateKey);
	ASSERT_EQ(actualSignLength, crypto_sign_BYTES);
	//printf("signature: %s\n", sign->convertToHex().data());
	transaction.signatureMap.push({ keyPair.publicKey, sign });
	transaction.bodyBytes = bodyBytes;

	toJson::Context c(transaction);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"signatureMap\":[{\"pubkey\":\"25971aa0e7422144dcc244887e29ef160d5479b1219e9817ca6ece38b09f37c0\",\"signature\":\"d9d80d166694a921bc489e6bf7118aac2bd9b1312e5910c5c6f08c3617cb79c2dc68229fb268bacf258bd80b7d0a67517f45ce1adba8e88c12f42a562b0d1d05\"}],\"bodyBytes\":{\"json\":\"cannot deserialize from body bytes\"}}");
}


TEST(ToJsonTest, MinimalConfirmedTransaction) {

	ConfirmedTransaction confirmedTransaction(
		7,
		std::make_unique<GradidoTransaction>(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		make_shared<memory::Block>(32),
		"179.00"
	);
	toJson::Context c(confirmedTransaction);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[],\"bodyBytes\":{\"json\":\"mData is empty\"}},\"confirmedAt\":\"2021-01-01 01:22:10.0000\",\"versionNumber\":\"3.3\",\"runningHash\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"messageId\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"accountBalance\":\"179.0000\"}");
}

TEST(ToJsonTest, CompleteConfirmedTransaction) {
	std::string memo("Danke fuer dein Sein!");
	TransactionBodyBuilder bodyBuilder;
	auto transactionBody = bodyBuilder
		.setTransactionTransfer(
			g_KeyPairs[4].publicKey, // sender
			"100.251621",
			"",
			g_KeyPairs[5].publicKey // recipient
		)
		.setCreatedAt(createdAt)
		.setMemo(memo)
		.build();

	GradidoTransactionBuilder builder;
	auto keyPair = make_shared<KeyPairEd25519>(g_KeyPairs[0].publicKey, g_KeyPairs[0].privateKey);
	auto gradidoTransaction = builder
		.setTransactionBody(std::move(transactionBody))
		.sign(keyPair)
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		VERSION_STRING,
		nullptr,
		make_shared<memory::Block>(32),
		"899.748379"
	);
	confirmedTransaction.runningHash = std::make_shared<memory::Block>(confirmedTransaction.calculateRunningHash());
	toJson::Context c(confirmedTransaction, toJson::BodyBytesFormat::BASE64 | toJson::BodyBytesFormat::HEX | toJson::BodyBytesFormat::JSON);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[{\"pubkey\":\"643c438776fc2634faf887df8485b9ed580729c2099e00e4d4d53cd74626a0d6\",\"signature\":\"04b195f8b28fbb09bbefb63df2c48a0898f24fbeb84f42e42bd24c0251fc31c55f1386b9f10243464455570f2cfbdefbef411cbb8d7037af7dff6765d9b33209\"}],\"bodyBytes\":{\"base64\":\"ChVEYW5rZSBmdWVyIGRlaW4gU2VpbiESCAiAzLn/BRAAGgMzLjMgADJSCi4KIIqMkyk8uX6HhBeNqK5YgUT3yYL0ZYv9NRAaHitHnD5XEggxMDAuMjUxNhoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7Fdg==\",\"hex\":\"0a1544616e6b652066756572206465696e205365696e2112080880ccb9ff0510001a03332e33200032520a2e0a208a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e5712083130302e323531361a001220d1a95824c8485900279b92a60175fc676f8914c61d7399c66c2d0cb6fa9ec576\",\"json\":{\"memo\":\"Danke fuer dein Sein!\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57\",\"amount\":\"100.2516\"},\"recipient\":\"d1a95824c8485900279b92a60175fc676f8914c61d7399c66c2d0cb6fa9ec576\"}}}},\"confirmedAt\":\"2021-01-01 01:22:10.0000\",\"versionNumber\":\"3.3\",\"runningHash\":\"882700c5d5acc7381a9da4861edb90adbbabc7d642869fdd572b345e5665d85a\",\"messageId\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"accountBalance\":\"899.7484\"}");
	// auto jsonPretty = c.run(true);
	// printf("json pretty: %s\n", jsonPretty.data());
}