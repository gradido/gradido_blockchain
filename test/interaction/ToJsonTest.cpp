#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/serialization/toJsonString.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum_flags.hpp"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;
using namespace serialization;

TEST(ToJsonTest, TransactionBodyWithoutMemo)
{
	TransactionBody body(createdAt, VERSION_STRING);

	EXPECT_EQ(toJsonString(body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\"}");

	// printf("json pretty: %s\n", jsonPretty.data());
	EXPECT_EQ(toJsonString(body, true), "{\n    \"memos\": [],\n    \"createdAt\": \"2021-01-01 00:00:00.0000\",\n    \"versionNumber\": \"3.5\",\n    \"type\": \"LOCAL\"\n}");
}

TEST(ToJsonTest, CommunityRootBody)
{
	GradidoTransactionBuilder builder;
	builder
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

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"communityRoot\":{\"pubkey\":\"81670329946988edf451f4c424691d83cf5a90439042882d5bb72243ef551ef4\",\"gmwPubkey\":\"d7e3a8a090aa44873246f5c6acfc17ff74ee174f56e7bd2a55ffb81041f6db1d\",\"aufPubkey\":\"946f583630d89c77cc1fc61d46726a3adeacb91ccab166c08a44ca0a0a0255c4\"}}");
}

TEST(ToJsonTest, RegisterAddressBody) {
	GradidoTransactionBuilder builder;
	builder
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

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"registerAddress\":{\"userPubkey\":\"f4dd3989f7554b7ab32e3dd0b7f9e11afce90a1811e9d1f677169eb44bf44272\",\"addressType\":\"COMMUNITY_HUMAN\",\"accountPubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"derivationIndex\":1}}");
}

TEST(ToJsonTest, GradidoCreationBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000)),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	EXPECT_EQ(body->getMemos().size(), 1);
	EXPECT_TRUE(body->getMemos()[0].isPlain());
	
	EXPECT_EQ(toJsonString(*body), "{\"memos\":[{\"type\":\"PLAIN\",\"memo\":\"Deine erste Schoepfung ;)\"}],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"creation\":{\"recipient\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"1000.0000\"},\"targetDate\":\"2020-12-31 23:56:40.0000\"}}");
}

TEST(ToJsonTest, GradidoTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500)),
			g_KeyPairs[5]->getPublicKey()
		)
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	
	EXPECT_EQ(toJsonString(*body), "{\"memos\":[{\"type\":\"PLAIN\",\"memo\":\"Ich teile mit dir\"}],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"500.5500\"},\"recipient\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\"}}");
}

TEST(ToJsonTest, GradidoDeferredTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(deferredTransferMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5555500)),
				g_KeyPairs[5]->getPublicKey()
			), DurationSeconds(std::chrono::seconds(5784))
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();

	ASSERT_TRUE(body->isDeferredTransfer());

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[{\"type\":\"PLAIN\",\"memo\":\"Link zum einloesen\"}],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"deferredTransfer\":{\"transfer\":{\"sender\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"555.5500\"},\"recipient\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\"},\"timeout\":\"1 hours 36 minutes 24 seconds\"}}");
}

TEST(ToJsonTest, CommunityFriendsUpdateBody) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityFriendsUpdate(true)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	
	ASSERT_TRUE(body->isCommunityFriendsUpdate());

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"communityFriendsUpdate\":{\"colorFusion\":true}}");
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

	EXPECT_EQ(toJsonString(transaction), "{\"signatureMap\":[{\"pubkey\":\"f4dd3989f7554b7ab32e3dd0b7f9e11afce90a1811e9d1f677169eb44bf44272\",\"signature\":\"b4c8d994c7c08a6b13685d33767fc843061a6bcfa0d3c415335567610c0deeaa45efce6e038ca7c1d21bcfba98b0f6fa9ed6c75f9cda6ce186db400120c09a02\"}],\"bodyBytes\":\"cannot deserialize from body bytes\"}");
}


TEST(ToJsonTest, MinimalConfirmedTransaction) {

	ConfirmedTransaction confirmedTransaction(
		7,
		std::make_unique<GradidoTransaction>(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		make_shared<memory::Block>(32),
		{}
	);

	EXPECT_EQ(toJsonString(confirmedTransaction), "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[],\"bodyBytes\":\"body bytes missing\"},\"confirmedAt\":\"2021-01-01 01:22:10.0000\",\"versionNumber\":\"3.5\",\"runningHash\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"messageId\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"accountBalances\":[]}");
}

TEST(ToJsonTest, CompleteConfirmedTransaction) {
	EncryptedMemo memo("Danke fuer dein Sein!");

	GradidoTransactionBuilder builder;
	auto gradidoTransaction = builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				GradidoUnit::fromGradidoCent(1002516),
				""
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.addMemo(memo)
		.sign(g_KeyPairs[0])
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(32),
		{ 
			{ g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000) },
			{ g_KeyPairs[5]->getPublicKey(), GradidoUnit::fromGradidoCent(8997483) }
		}
	);
	EXPECT_EQ(toJsonString(confirmedTransaction), "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[{\"pubkey\":\"81670329946988edf451f4c424691d83cf5a90439042882d5bb72243ef551ef4\",\"signature\":\"04e0d0f6c4bbd2d87dc879fc5f72be48dbf682c888757fd5d3d6da0af4026fec848f60ddfdfcd284862a7f7a68a08330274d4190325d346059b39303cc40240a\"}],\"bodyBytes\":{\"memos\":[{\"type\":\"PLAIN\",\"memo\":\"Danke fuer dein Sein!\"}],\"createdAt\":\"2021-01-01 00:00:00.0000\",\"versionNumber\":\"3.5\",\"type\":\"LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"amount\":\"100.2516\"},\"recipient\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\"}}},\"confirmedAt\":\"2021-01-01 01:22:10.0000\",\"versionNumber\":\"3.5\",\"runningHash\":\"28a58de12318789f59ee15373a1ef8337da0e2cd66f266bf756590ffb5447ecc\",\"messageId\":\"0000000000000000000000000000000000000000000000000000000000000000\",\"accountBalances\":[{\"pubkey\":\"db0ed6125a14f030abed1bfc831e0a218cf9fabfcee7ecd581c0c0e788f017c7\",\"balance\":\"100.0000\"},{\"pubkey\":\"244d28d7cc5be8fe8fb0d8e1d1b90de7603386082d793ce8874f6357e6e532ad\",\"balance\":\"899.7483\"}]}");	
	// printf("json pretty: %s\n", toJsonString(confirmedTransaction, true).data());
}