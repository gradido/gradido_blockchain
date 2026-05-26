#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/serialization/toJsonString.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain_core/types/balance_derivation.h"

#include "magic_enum/magic_enum_flags.hpp"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;
using namespace serialization;

TEST(ToJsonTest, TransactionBodyWithoutMemo)
{
	TransactionBody body(createdAt, 0);

	EXPECT_EQ(toJsonString(body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\"}");

	// printf("json pretty: %s\n", jsonPretty.data());
	EXPECT_EQ(toJsonString(body, true), "{\n    \"memos\": [],\n    \"createdAt\": \"2021-01-01 00:00:00.0000Z\",\n    \"type\": \"GRDT_CROSS_GROUP_LOCAL\"\n}");
}

TEST(ToJsonTest, CommunityRootBody)
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

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"communityRoot\":{\"pubkey\":\"aad12ba826b9114457554c4c557fa3a878af9f21f81621b0c3e6161506112829\",\"gmwPubkey\":\"14b8a6d8563bbb3597df7d8275b70b56a0461a5b791d63c08cc05947d2061c89\",\"aufPubkey\":\"466fdd73bb81627f360fc8e68b3dea7fc02094b3a6f1cd2adfbd39a4f32e259e\"}}");
}

TEST(ToJsonTest, RegisterAddressBody) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setSenderCommunity(communityId)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			GRDT_ADDRESS_COMMUNITY_HUMAN,
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

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"registerAddress\":{\"userPubkey\":\"bc7125b7e4b7b534d232c215c2cbce344088604a0e8aee3932e9bf416a06cffe\",\"addressType\":\"GRDT_ADDRESS_COMMUNITY_HUMAN\",\"nameHash\":\"1fe41f7966a4b61a963886a0fa4d136d8579424d9c02a2751efba0e8e5559f61\",\"accountPubkey\":\"eff2919350a53ac80f898062b7c67ff8028245f4cc44e08ef22f885171554f3b\",\"derivationIndex\":1}}");
}

TEST(ToJsonTest, GradidoCreationBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), communityIdIndex),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
		.sign(g_KeyPairs[6])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	EXPECT_EQ(body->getMemos().size(), 1);
	EXPECT_TRUE(body->getMemos()[0].isPlain());
	
	EXPECT_EQ(toJsonString(*body), "{\"memos\":[{\"type\":\"GRDT_MEMO_KEY_PLAIN\",\"memo\":\"Deine erste Schoepfung ;)\"}],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"creation\":{\"recipient\":{\"pubkey\":\"eff2919350a53ac80f898062b7c67ff8028245f4cc44e08ef22f885171554f3b\",\"amount\":\"1000.0000\",\"coinCommunityId\":\"019e347c-540a-73d2-9886-7fece00d5a2e\"},\"targetDate\":\"2020-12-31 23:56:40.0000Z\"}}");
}

TEST(ToJsonTest, GradidoTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	
	EXPECT_EQ(toJsonString(*body), "{\"memos\":[{\"type\":\"GRDT_MEMO_KEY_PLAIN\",\"memo\":\"Ich teile mit dir\"}],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"eff2919350a53ac80f898062b7c67ff8028245f4cc44e08ef22f885171554f3b\",\"amount\":\"500.5500\",\"coinCommunityId\":\"019e347c-540a-73d2-9886-7fece00d5a2e\"},\"recipient\":\"7ef57ee34e4cae5e014034b7f8043ba640d1a26f07d622ac342e64ad75b9f60d\"}}");
}

TEST(ToJsonTest, GradidoDeferredTransferBody) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(deferredTransferMemoString)
		.setCreatedAt(createdAt)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5555500), communityIdIndex),
				g_KeyPairs[5]->getPublicKey()
			), DurationSeconds(std::chrono::seconds(5784))
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();

	ASSERT_TRUE(body->isDeferredTransfer());

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[{\"type\":\"GRDT_MEMO_KEY_PLAIN\",\"memo\":\"Link zum einloesen\"}],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"deferredTransfer\":{\"transfer\":{\"sender\":{\"pubkey\":\"eff2919350a53ac80f898062b7c67ff8028245f4cc44e08ef22f885171554f3b\",\"amount\":\"555.5500\",\"coinCommunityId\":\"019e347c-540a-73d2-9886-7fece00d5a2e\"},\"recipient\":\"7ef57ee34e4cae5e014034b7f8043ba640d1a26f07d622ac342e64ad75b9f60d\"},\"timeout\":\"1 hours 36 minutes 24 seconds\"}}");
}

TEST(ToJsonTest, CommunityFriendsUpdateBody) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setCommunityFriendsUpdate(true)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	
	ASSERT_TRUE(body->isCommunityFriendsUpdate());

	EXPECT_EQ(toJsonString(*body), "{\"memos\":[],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"communityFriendsUpdate\":{\"colorFusion\":true}}");
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
	GradidoTransaction transaction(signatures, bodyBytes, 0);
	EXPECT_EQ(toJsonString(transaction), "{\"signatureMap\":[{\"pubkey\":\"bc7125b7e4b7b534d232c215c2cbce344088604a0e8aee3932e9bf416a06cffe\",\"signature\":\"7134e8800080e7c33e1b59004045e879c8c75c9f372611dc72b5d39a393b30117754e117297c809264e1475d7a008ebbc73ecb66d69cd1048bf582d0a4df850b\"}],\"bodyBytes\":\"cannot deserialize from body bytes\",\"pairingLedgerAnchor\":{\"type\":\"GRDT_LEDGER_ANCHOR_UNSPECIFIED\",\"value\":null}}");
}

TEST(ToJsonTest, CompleteConfirmedTransaction) {
	EncryptedMemo memo("Danke fuer dein Sein!");

	GradidoTransactionBuilder builder;
	auto gradidoTransaction = builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4]->getPublicKey(), // sender
				GradidoUnit::fromGradidoCent(1002516),
				communityIdIndex
			), g_KeyPairs[5]->getPublicKey() // recipient
		)
		.setCreatedAt(createdAt)
		.addMemo(memo)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[0])
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		LedgerAnchor(7, GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID),
		{ 
			{ g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000), communityIdIndex},
			{ g_KeyPairs[5]->getPublicKey(), GradidoUnit::fromGradidoCent(8997483), communityIdIndex}
		},
		GRDT_BALANCE_DERIVATION_EXTERN
	);
	EXPECT_EQ(toJsonString(confirmedTransaction), "{\"id\":7,\"gradidoTransaction\":{\"signatureMap\":[{\"pubkey\":\"aad12ba826b9114457554c4c557fa3a878af9f21f81621b0c3e6161506112829\",\"signature\":\"53eaf619b7ef3fea85f7b4d36d5a874e76d7e62ba8238941f400ece9de94a1bc473759eff58c50219f365eeea5c7940cdd29a8147970efeaac2ce3f5c3d59b02\"}],\"bodyBytes\":{\"memos\":[{\"type\":\"GRDT_MEMO_KEY_PLAIN\",\"memo\":\"Danke fuer dein Sein!\"}],\"createdAt\":\"2021-01-01 00:00:00.0000Z\",\"type\":\"GRDT_CROSS_GROUP_LOCAL\",\"transfer\":{\"sender\":{\"pubkey\":\"eff2919350a53ac80f898062b7c67ff8028245f4cc44e08ef22f885171554f3b\",\"amount\":\"100.2516\",\"coinCommunityId\":\"019e347c-540a-73d2-9886-7fece00d5a2e\"},\"recipient\":\"7ef57ee34e4cae5e014034b7f8043ba640d1a26f07d622ac342e64ad75b9f60d\"}},\"pairingLedgerAnchor\":{\"type\":\"GRDT_LEDGER_ANCHOR_UNSPECIFIED\",\"value\":null}},\"confirmedAt\":\"2021-01-01 01:22:10.0000Z\",\"runningHash\":\"f3320e9fd77fa2e9098c9c7a43774eac8e7b196f9145502a6014ba47fa3180bc\",\"ledgerAnchor\":{\"type\":\"GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID\",\"value\":7},\"accountBalances\":[{\"pubkey\":\"eff2919350a53ac80f898062b7c67ff8028245f4cc44e08ef22f885171554f3b\",\"balance\":\"100.0000\",\"coinCommunityId\":\"019e347c-540a-73d2-9886-7fece00d5a2e\"},{\"pubkey\":\"7ef57ee34e4cae5e014034b7f8043ba640d1a26f07d622ac342e64ad75b9f60d\",\"balance\":\"899.7483\",\"coinCommunityId\":\"019e347c-540a-73d2-9886-7fece00d5a2e\"}],\"balanceDerivationType\":\"GRDT_BALANCE_DERIVATION_EXTERN\"}");	
	// printf("json pretty: %s\n", toJsonString(confirmedTransaction, true).data());
}