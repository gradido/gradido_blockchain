#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"
#include "const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

using namespace gradido;
using namespace data;
using namespace interaction;

TEST(SerializeTest, TransactionBodyWithoutMemo)
{
	TransactionBody body(std::string(), createdAt, VERSION_STRING);
	serialize::Context c(body);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), "CgASCAiAzLn/BRAAGgMzLjMgAA==");
}

TEST(SerializeTest, TransactionBody)
{
	TransactionBody body("memo", createdAt, VERSION_STRING);
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(serialized->convertToBase64(), "CgRtZW1vEggIgMy5/wUQABoDMy4zIAA=");
}

TEST(SerializeTest, CommunityRootBody) 
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
	ASSERT_EQ(body->getType(), data::CrossGroupType::LOCAL);
	
	serialize::Context c(*transaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), communityRootTransactionBase64);
}

TEST(SerializeTest, RegisterAddressBody) {
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
	serialize::Context c(*transaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(serialized->convertToBase64(), registeAddressTransactionBase64);
}

TEST(SerializeTest, GradidoCreationBody) {
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
	serialize::Context c(*transaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(), creationTransactionBase64);
}

TEST(SerializeTest, GradidoTransferBody) {
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
	serialize::Context c(*transaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(), transferTransactionBase64);
}

TEST(SerializeTest, GradidoDeferredTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Link zum einloesen")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "555.55"),
				g_KeyPairs[5]->getPublicKey()
			),
			timeout
		)
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	serialize::Context c(*transaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(), deferredTransferTransactionBase64);
}

TEST(SerializeTest, CommunityFriendsUpdateBody) {
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
	serialize::Context c(*transaction);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), communityFriendsUpdateBase64);
}

TEST(SerializeTest, GradidoTransaction) {
	auto bodyBytes = make_shared<memory::Block>(invalidBodyTestPayload);
	//printf("signature: %s\n", sign->convertToHex().data());
	SignatureMap signatureMap;
	signatureMap.push({ g_KeyPairs[3]->getPublicKey(), make_shared<memory::Block>(g_KeyPairs[3]->sign(*bodyBytes))});
	GradidoTransaction transaction(signatureMap, bodyBytes);

	serialize::Context c(transaction);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), gradidoTransactionSignedInvalidBody);
}

TEST(SerializeTest, SignatureMap) {
	memory::Block message(
		"Human nature is a complex interplay of light and shadow, where our greatest strengths often emerge from our deepest vulnerabilities. To be human is to strive for connection, seek meaning, and continuously evolve through both triumphs and trials."
	);
	
	unsigned long long actualSignLength = 0;
	SignatureMap signatureMap;
	for (int i = 0; i < 2; i++) {
		signatureMap.push({ g_KeyPairs[i]->getPublicKey(), make_shared<memory::Block>(g_KeyPairs[i]->sign(message))});
	}

	serialize::Context c(signatureMap);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CmQKIIFnAymUaYjt9FH0xCRpHYPPWpBDkEKILVu3IkPvVR70EkAE4ND2xLvS2H3Iefxfcr5I2/aCyIh1f9XT1toK9AJv7Mfu5U7ftL87CuOGbn+VfdemtvbIBr9amXNo4HI+BIYICmQKINfjqKCQqkSHMkb1xqz8F/907hdPVue9KlX/uBBB9tsdEkD82U+ofaXHmbiWJUf/cYiyb7xKW9MrNAjPlelL7a5+2WjM4mBPneSPn7iCK3ewWQXbN/OF6As/gWqQI7Mq7RAF"
	);
}

TEST(SerializeTest, MinimalConfirmedTransaction) {
	
	ConfirmedTransaction confirmedTransaction(
		7,
		std::make_unique<GradidoTransaction>(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		make_shared<memory::Block>(32),
		"179.00"
	);
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), minimalConfirmedTransaction);
}

TEST(SerializeTest, CompleteConfirmedTransaction) {
	std::string memo("Danke fuer dein Sein!");

	GradidoTransactionBuilder builder;
	auto gradidoTransaction = builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				"100.251621"
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.setMemo(memo)
		.setVersionNumber(VERSION_STRING)
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
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	// printf("running hash: %s\n", confirmedTransaction.getRunningHash()->convertToHex().data());
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), completeConfirmedTransaction);
}