#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"
#include "const.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/data/hiero/TopicId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Type.h"
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
	hiero::TransactionId transactionId = defaultHieroTransactionId;
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

TEST(SerializeTest, CommunityRootBody)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setSenderCommunity(communityId)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey()->data(),
			g_KeyPairs[1]->getPublicKey()->data(),
			g_KeyPairs[2]->getPublicKey()->data()
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
		.setSenderCommunity(communityId)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			make_shared<const Block>(g_KeyPairs[3]->getPublicKey()->calculateHash()),
			g_KeyPairs[4]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[3])
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
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), 0),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
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
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5555500), 0),
				g_KeyPairs[5]->getPublicKey()
			),
			timeoutDuration
		)
		.setSenderCommunity(communityId)
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
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setCommunityFriendsUpdate(true)
		.setSenderCommunity(communityId)
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
	GradidoTransaction transaction(signatureMap, bodyBytes, 0);

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

TEST(SerializeTest, CompleteConfirmedTransaction) {
	GradidoTransactionBuilder builder;
	auto gradidoTransaction = builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				GradidoUnit::fromGradidoCent(1002516),
				0
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.addMemo(completeTransactionMemoString)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[0])
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		LedgerAnchor(defaultHieroTransactionId),
		{
			{ g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000), 0},
			{ g_KeyPairs[5]->getPublicKey(), GradidoUnit::fromGradidoCent(8997483), 0}
		},
		BalanceDerivationType::EXTERN
	);
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	// printf("running hash: %s\n", confirmedTransaction.getRunningHash()->convertToHex().data());
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(), completeConfirmedTransaction);
}

TEST(SerializeTest, CrossGroupTransactions) {
	auto community2 = "test2";
	GradidoTransactionBuilder builder;
	builder
		.setSenderCommunity(communityId)
		.setRecipientCommunity(community2)
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				GradidoUnit::fromGradidoCent(1002516),
				0
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.addMemo(completeTransactionMemoString)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.sign(g_KeyPairs[0]);
	
	LedgerAnchor senderLedgerAnchor(defaultHieroTransactionId);

	ConfirmedTransaction confirmedTransactionOutbound(
		7,
		std::move(builder.buildOutbound()),
		confirmedAt,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		LedgerAnchor(defaultHieroTransactionId),
		{
			{ g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000), 0},
			{ g_KeyPairs[5]->getPublicKey(), GradidoUnit::fromGradidoCent(8997483), 0}
		},
		BalanceDerivationType::EXTERN
	);
	serialize::Context cOutbound(confirmedTransactionOutbound);
	auto serializedOutbound = cOutbound.run();
	// printf("serialized outbound: %s\n", serializedOutbound->convertToBase64().c_str());
	ASSERT_STREQ(
		serializedOutbound->convertToBase64().c_str(),
		"CAcS6QEKZgpkCiCBZwMplGmI7fRR9MQkaR2Dz1qQQ5BCiC1btyJD71Ue9BJABODQ9sS70th9yHn8X3K+SNv2gsiIdX/V09baCvQCb+z5+Tjbhx/eNy9tgFgVJzRoWuIfUwKQvMIpDFok/+fNDBJ9ChkIAhIVRGFua2UgZnVlciBkZWluIFNlaW4hEgYIgMy5/wUaAzMuNSACKgV0ZXN0MjJKCiYKINsO1hJaFPAwq+0b/IMeCiGM+fq/zufs1YHAwOeI8BfHEJSYPRIgJE0o18xb6P6PsNjh0bkN52AzhggteTzoh09jV+blMq0aABoGCMLyuf8FIgMzLjcqIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMhUIAhoRCgkIqemnUhD+4wESBBj8sgc6Jgog2w7WEloU8DCr7Rv8gx4KIYz5+r/O5+zVgcDA54jwF8cQwIQ9OicKICRNKNfMW+j+j7DY4dG5DedgM4YILXk86IdPY1fm5TKtEOuUpQRAAg=="
	);
	deserialize::Context cDeserializeOutbound(serializedOutbound, deserialize::Type::CONFIRMED_TRANSACTION);
	cDeserializeOutbound.run(g_appContext->getOrAddCommunityIdIndex(communityId));
	ASSERT_TRUE(cDeserializeOutbound.isConfirmedTransaction());	
	auto confirmedOutbound = cDeserializeOutbound.getConfirmedTransaction();

	builder.setParentLedgerAnchor(senderLedgerAnchor);

	ConfirmedTransaction confirmedTransactionInbound(
		7,
		std::move(builder.buildInbound()),
		confirmedAt,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		LedgerAnchor({ confirmedAt, {0, 0, 2} }),
		{
			{ g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000), 0},
			{ g_KeyPairs[5]->getPublicKey(), GradidoUnit::fromGradidoCent(8997483), 0}
		},
		BalanceDerivationType::EXTERN
	);
	serialize::Context cInbound(confirmedTransactionInbound);
	auto serializedInbound = cInbound.run();
	// printf("serialized inbound: %s\n", serializedInbound->convertToBase64().c_str());
	ASSERT_STREQ(
		serializedInbound->convertToBase64().c_str(),
		"CAcSmAIKZgpkCiCBZwMplGmI7fRR9MQkaR2Dz1qQQ5BCiC1btyJD71Ue9BJABODQ9sS70th9yHn8X3K+SNv2gsiIdX/V09baCvQCb+xrpCtIYAa2zlVoEbj2EVoyhcUuQqFW5MZJioXSy3fQAhKWAQoZCAISFURhbmtlIGZ1ZXIgZGVpbiBTZWluIRIGCIDMuf8FGgMzLjUgASoOdGVzdC1jb21tdW5pdHkyWgo2CiDbDtYSWhTwMKvtG/yDHgohjPn6v87n7NWBwMDniPAXxxCUmD0aDnRlc3QtY29tbXVuaXR5EiAkTSjXzFvo/o+w2OHRuQ3nYDOGCC15POiHT2NX5uUyrRoVCAIaEQoJCKnpp1IQ/uMBEgQY/LIHGgYIwvK5/wUiAzMuNyogAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyEAgCGgwKBgjC8rn/BRICGAI6NQog2w7WEloU8DCr7Rv8gx4KIYz5+r/O5+zVgcDA54jwF8cQwIQ9Gg10ZXN0LWNvbW11bml0OjYKICRNKNfMW+j+j7DY4dG5DedgM4YILXk86IdPY1fm5TKtEOuUpQQaDXRlc3QtY29tbXVuaXRAAg=="
	);
	deserialize::Context cDeserializeInbound(serializedInbound, deserialize::Type::CONFIRMED_TRANSACTION);
	cDeserializeInbound.run(g_appContext->getOrAddCommunityIdIndex(community2));
	ASSERT_TRUE(cDeserializeInbound.isConfirmedTransaction());
	auto confirmedInbound = cDeserializeInbound.getConfirmedTransaction();

	ASSERT_TRUE(confirmedInbound->getGradidoTransaction()->isPairing(*confirmedOutbound->getGradidoTransaction()));

	
	// printf("running hash: %s\n", confirmedTransaction.getRunningHash()->convertToHex().data());
	// printf("serialized size: %llu, serialized in base64: %s\n", serialized->size(), serialized->convertToBase64().data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	// ASSERT_EQ(serialized->convertToBase64(), completeConfirmedTransaction);
}

TEST(SerializeTest, TransactionTriggerEvent) {
	TransactionTriggerEvent transactionTriggerEvent(12, confirmedAt, data::TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL);
	auto serialized = serialize::Context(transactionTriggerEvent).run();
	ASSERT_EQ(serialized->convertToBase64(), "CAwSCAjC8rn/BRAAGAE=");
}