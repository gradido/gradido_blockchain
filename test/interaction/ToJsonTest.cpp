#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/toJson/Context.h"
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
	GradidoTransactionBuilder builder;
	builder
		.setMemo("")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());

	toJson::Context c(*body);
	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"communityRoot\":{\"pubkey\":\"81670329946988edf451f4c424691d83cf5a90439042882d5bb72243ef551ef4\",\"gmwPubkey\":\"d7e3a8a090aa44873246f5c6acfc17ff74ee174f56e7bd2a55ffb81041f6db1d\",\"aufPubkey\":\"946f583630d89c77cc1fc61d46726a3adeacb91ccab166c08a44ca0a0a0255c4\"}}");
}

TEST(ToJsonTest, RegisterAddressBody) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[4]->getPublicKey()	
		)
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isRegisterAddress());

	toJson::Context c(*body);
	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"registerAddress\":{\"userPubkey\":\"f4dd3989f7554b7ab32e3dd0b7f9e11afce90a1811e9d1f677169eb44bf44272\",\"addressType\":\"COMMUNITY_HUMAN\",\"accountPubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"derivationIndex\":1}}");
}

TEST(ToJsonTest, GradidoCreationBody) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Deine erste Schoepfung ;)")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());

	toJson::Context c(*body);
	auto json = c.run();
	// printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"Deine erste Schoepfung ;)\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"creation\":{\"recipient\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"1000.0000\"},\"targetDate\":\"2020-12-31 23:56:40.0000\"}}");
}

TEST(ToJsonTest, GradidoTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	toJson::Context c(*body);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"Ich teile mit dir\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"500.5500\"},\"recipient\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\"}}");
}

TEST(ToJsonTest, GradidoDeferredTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Link zum einloesen")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "555.55"),
				g_KeyPairs[5]->getPublicKey()
			), TimestampSeconds(1609465000)
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();

	ASSERT_TRUE(body->isDeferredTransfer());
	toJson::Context c(*body);

	auto json = c.run();
	// printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"Link zum einloesen\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"deferredTransfer\":{\"transfer\":{\"sender\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"555.5500\"},\"recipient\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\"},\"timeout\":\"2021-01-01 01:36:40.0000\"}}");
}

TEST(ToJsonTest, CommunityFriendsUpdateBody) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityFriendsUpdate(true)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	
	ASSERT_TRUE(body->isCommunityFriendsUpdate());
	toJson::Context c(*body);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"memo\":\"\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"communityFriendsUpdate\":{\"colorFusion\":true}}");
}

TEST(ToJsonTest, GradidoTransaction) {	
	auto bodyBytes = make_shared<memory::Block>(
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson "
	);
	SignatureMap signatures;
	signatures.push(SignaturePair(
		g_KeyPairs[3]->getPublicKey(),
		std::make_shared<memory::Block>(g_KeyPairs[3]->sign(*bodyBytes))
	));
	GradidoTransaction transaction(signatures, bodyBytes);
	toJson::Context c(transaction);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"signatureMap\":[{\"pubkey\":\"f4dd3989f7554b7ab32e3dd0b7f9e11afce90a1811e9d1f677169eb44bf44272\",\"signature\":\"b4c8d994c7c08a6b13685d33767fc843061a6bcfa0d3c415335567610c0deeaa45efce6e038ca7c1d21bcfba98b0f6fa9ed6c75f9cda6ce186db400120c09a02\"}],\"bodyBytes\":{\"json\":\"cannot deserialize from body bytes\"}}");
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
	EXPECT_EQ(json, "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[],\"bodyBytes\":{\"json\":\"body bytes missing\"}},\"confirmedAt\":\"2021-01-01 01:22:10.0000\",\"versionNumber\":\"3.3\",\"runningHash\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"messageId\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"accountBalance\":\"179.0000\"}");
}

TEST(ToJsonTest, CompleteConfirmedTransaction) {
	std::string memo("Danke fuer dein Sein!");

	GradidoTransactionBuilder builder;
	auto gradidoTransaction = builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				"100.251621",
				""
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.setMemo(memo)
		.sign(g_KeyPairs[0])
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(32),
		"899.748379"
	);
	toJson::Context c(confirmedTransaction, toJson::BodyBytesType::BASE64 | toJson::BodyBytesType::HEX | toJson::BodyBytesType::JSON);

	auto json = c.run();
	//printf("json: %s\n", json.data());
	EXPECT_EQ(json, "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[{\"pubkey\":\"81670329946988edf451f4c424691d83cf5a90439042882d5bb72243ef551ef4\",\"signature\":\"04e0d0f6c4bbd2d87dc879fc5f72be48dbf682c888757fd5d3d6da0af4026fec35e9221fbf52f66769e086e3b665fead1cf73da934748c88dec4f14304521b09\"}],\"bodyBytes\":{\"base64\":\"ChVEYW5rZSBmdWVyIGRlaW4gU2VpbiESCAiAzLn/BRAAGgMzLjMgADJSCi4KINsO1hJaFPAwq+0b/IMeCiGM+fq/zufs1YHAwOeI8BfHEggxMDAuMjUxNhoAEiAkTSjXzFvo/o+w2OHRuQ3nYDOGCC15POiHT2NX5uUyrQ==\",\"hex\":\"0a1544616e6b652066756572206465696e205365696e2112080880ccb9ff0510001a03332e33200032520a2e0a20db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c712083130302e323531361a001220244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\",\"json\":{\"memo\":\"Danke fuer dein Sein!\",\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.3\",\"type\":\"LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"100.2516\"},\"recipient\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\"}}}},\"confirmedAt\":\"2021-01-01 01:22:10.0000\",\"versionNumber\":\"3.3\",\"runningHash\":\"02c718c2d4154829e6e64ed4cb0aeebb5df4cb4f285f49cc299cb286da242afd\",\"messageId\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"accountBalance\":\"899.7484\"}");
	// auto jsonPretty = c.run(true);
	// printf("json pretty: %s\n", jsonPretty.data());
}