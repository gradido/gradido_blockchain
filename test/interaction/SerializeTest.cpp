#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"
#include "const.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"
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
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey,
			g_KeyPairs[2].publicKey
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	serialize::Context c(*body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), communityRootTransactionBase64);
}

TEST(SerializeTest, RegisterAddressBody) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[4].publicKey
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	serialize::Context c(*body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(serialized->convertToBase64(), registeAddressTransactionBase64);
}

TEST(SerializeTest, GradidoCreationBody) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("Deine erste Schoepfung ;)")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());
	serialize::Context c(*body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(), creationTransactionBase64);
}

TEST(SerializeTest, GradidoTransferBody) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[5].publicKey
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isTransfer());
	serialize::Context c(*body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(), transferTransactionBase64);
}

TEST(SerializeTest, GradidoDeferredTransferBody) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("Link zum einloesen")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "555.55"),
				g_KeyPairs[5].publicKey
			),
			timeout
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isDeferredTransfer());
	serialize::Context c(*body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(), deferredTransferTransactionBase64);
}

TEST(SerializeTest, CommunityFriendsUpdateBody) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityFriendsUpdate(true)
	;
	auto body = builder.build();

	ASSERT_TRUE(body->isCommunityFriendsUpdate());
	serialize::Context c(*body);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), communityFriendsUpdateBase64);
}

TEST(SerializeTest, GradidoTransaction) {
	auto bodyBytes = make_shared<memory::Block>(invalidBodyTestPayload);
	auto sign = make_shared<memory::Block>(crypto_sign_BYTES);
	auto& keyPair = g_KeyPairs[3];
	unsigned long long actualSignLength = 0;
	crypto_sign_detached(*sign, &actualSignLength, *bodyBytes, bodyBytes->size(), *keyPair.privateKey);
	ASSERT_EQ(actualSignLength, crypto_sign_BYTES);
	//printf("signature: %s\n", sign->convertToHex().data());
	SignatureMap signatureMap;
	signatureMap.push({ keyPair.publicKey, sign });
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
		auto sign = make_shared<memory::Block>(crypto_sign_BYTES);
		crypto_sign_detached(
			sign->data(), 
			&actualSignLength, 
			message.data(), 
			message.size(), 
			g_KeyPairs[i].privateKey->data()
		);
		signatureMap.push({ g_KeyPairs[i].publicKey, sign });
	}

	serialize::Context c(signatureMap);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CmQKIGQ8Q4d2/CY0+viH34SFue1YBynCCZ4A5NTVPNdGJqDWEkD2NqoGVja1mXQoKEneXryWf7flL8JpiBMwRN3sbSTcncJXQqrZ8/5yb83u8EPPJRL7zXj660DD1z6RQDw3ekIJCmQKIFH5sejZhHY63U2QzGQi8f1KCcZ3uY5LGYwFW8KUIPWOEkDkQn2qJW38uEFO8YnX8U/1/OSrI/DYSeTqTaecDDe4SyM/yKsPqcLZ54Lzv1Nba/ckiImrDBseH8d/EJTi57IH"
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
	TransactionBodyBuilder bodyBuilder;
	auto transactionBody = bodyBuilder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4].publicKey, // sender
				"100.251621"
			), g_KeyPairs[5].publicKey // recipient
		)
		.setCreatedAt(createdAt)
		.setMemo(memo)
		.setVersionNumber(VERSION_STRING)
		.build()
	;

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