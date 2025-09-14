#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"
#include "const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/data/hiero/TopicId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

using namespace gradido;
using namespace data;
using namespace interaction;

TEST(SerializeTest, HieroAccountId) 
{
	hiero::AccountId accountId(0, 0, 21231);
	serialize::Context c(accountId);
	auto serialized = c.run();
	ASSERT_EQ(serialized->convertToBase64(), "CAAQABjvpQE=");
	ASSERT_EQ(serialized->convertToHex(), "0800100018efa501");
}

TEST(SerializeTest, HieroTopicId)
{
	hiero::TopicId topicId(0, 0, 1621);
	serialize::Context c(topicId);
	auto serialized = c.run();
	ASSERT_EQ(serialized->convertToBase64(), "CAAQABjVDA==");
	ASSERT_EQ(serialized->convertToHex(), "0800100018d50c");
}

TEST(SerializeTest, HieroTransactionId)
{
	hiero::TransactionId transactionId("0.0.121212@172618921.29182");
	serialize::Context c(transactionId);
	auto serialized = c.run();
	ASSERT_EQ(serialized->convertToBase64(), std::string(hieroTransactionIdBase64));
	ASSERT_EQ(serialized->convertToHex(), "0a0908a9e9a75210fee30112080800100018fcb207");

	transactionId.setNonce(121);
	transactionId.setScheduled();
	serialized = c.run();
	ASSERT_EQ(serialized->convertToBase64(), "CgkIqemnUhD+4wESCAgAEAAY/LIHGAEgeQ==");
	ASSERT_EQ(serialized->convertToHex(), "0a0908a9e9a75210fee30112080800100018fcb20718012079");
}

TEST(SerializeTest, TransactionBodyWithoutMemo)
{
	TransactionBody body(createdAt, VERSION_STRING);
	serialize::Context c(body);
	auto serialized = c.run();
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), "EggIgMy5/wUQABoDMy41IAA=");
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
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
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
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	ASSERT_EQ(serialized->convertToBase64(), registeAddressTransactionBase64);
}

TEST(SerializeTest, GradidoCreationBody) {
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
	serialize::Context c(*transaction);
	auto serialized = c.run();
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	ASSERT_EQ(serialized->convertToBase64(), creationTransactionBase64);
}

TEST(SerializeTest, GradidoTransferBody) {
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
	serialize::Context c(*transaction);
	auto serialized = c.run();
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	ASSERT_EQ(serialized->convertToBase64(), transferTransactionBase64);
}

TEST(SerializeTest, GradidoDeferredTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(deferredTransferMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5555500)),
				g_KeyPairs[5]->getPublicKey()
			),
			timeoutDuration
		)
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	serialize::Context c(*transaction);
	auto serialized = c.run();
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
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
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
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
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
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
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
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
		make_shared<memory::Block>(memory::Block::fromBase64(hieroTransactionIdBase64)),
		{}
	);
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), minimalConfirmedTransaction);
}

TEST(SerializeTest, CompleteConfirmedTransaction) {
	GradidoTransactionBuilder builder;
	auto gradidoTransaction = builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				GradidoUnit::fromGradidoCent(1002516)
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.addMemo(completeTransactionMemoString)
		.setVersionNumber(VERSION_STRING)
		.sign(g_KeyPairs[0])
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(memory::Block::fromBase64(hieroTransactionIdBase64)),
		{
			{ g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000) },
			{ g_KeyPairs[5]->getPublicKey(), GradidoUnit::fromGradidoCent(8997483) }
		}
	);
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	// printf("running hash: %s\n", confirmedTransaction.getRunningHash()->convertToHex().data());
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), completeConfirmedTransaction);
}

TEST(SerializeTest, TransactionTriggerEvent) {
	TransactionTriggerEvent transactionTriggerEvent(12, confirmedAt, data::TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL);
	auto serialized = serialize::Context(transactionTriggerEvent).run();
	ASSERT_EQ(serialized->convertToBase64(), "CAwSCAjC8rn/BRAAGAE=");
}