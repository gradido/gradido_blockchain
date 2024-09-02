
#include "gtest/gtest.h"
#include "KeyPairs.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"

using namespace gradido;
using namespace gradido::data;
using namespace std::chrono;


TEST(TransactionBodyBuilderTest, CommunityRoot) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	builder
		.setCommunityRoot(g_KeyPairs[0].publicKey, g_KeyPairs[1].publicKey, g_KeyPairs[2].publicKey)
	;
	auto transactionBody = builder.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_TRUE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	auto communityRoot = transactionBody->getCommunityRoot();
	
	EXPECT_EQ(*communityRoot->getPubkey(), *g_KeyPairs[0].publicKey);
	EXPECT_EQ(*communityRoot->getGmwPubkey(), *g_KeyPairs[1].publicKey);
	EXPECT_EQ(*communityRoot->getAufPubkey(), *g_KeyPairs[2].publicKey);
	EXPECT_GT(transactionBody->getCreatedAt(), Timestamp(now));
}

TEST(TransactionBodyBuilderTest, RegisterAddressUserOnly) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	builder
		.setRegisterAddress(g_KeyPairs[3].publicKey, AddressType::COMMUNITY_HUMAN)
		.setCreatedAt(now)
	;
	auto transactionBody = builder.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_TRUE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->getMemo(), "");
	EXPECT_EQ(transactionBody->getCreatedAt(), Timestamp(now));

	auto registerAddress = transactionBody->getRegisterAddress();
	EXPECT_EQ(registerAddress->getAddressType(), AddressType::COMMUNITY_HUMAN);
	EXPECT_EQ(*registerAddress->getUserPublicKey(), *g_KeyPairs[3].publicKey);

}

TEST(TransactionBodyBuilderTest, RegisterAddressUserAndAccount) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	builder
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr, 
			g_KeyPairs[4].publicKey
		)
		.setCreatedAt(now)
	;
	auto transactionBody = builder.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_TRUE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->getMemo(), "");
	EXPECT_EQ(transactionBody->getCreatedAt(), Timestamp(now));

	auto registerAddress = transactionBody->getRegisterAddress();
	EXPECT_EQ(registerAddress->getAddressType(), AddressType::COMMUNITY_HUMAN);
	EXPECT_EQ(*registerAddress->getUserPublicKey(), *g_KeyPairs[3].publicKey);
	EXPECT_EQ(*registerAddress->getAccountPublicKey(), *g_KeyPairs[4].publicKey);
}

TEST(TransactionBodyBuilderTest, GradidoCreation) {
	auto now = system_clock::now();
	std::string memo("Du bist der Schoepfer deiner Welt!");
	TransactionBodyBuilder builder;
	builder
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000"),
			TimestampSeconds(1660953712)
		)
		.setCreatedAt(now)
		.setMemo(memo)
	;
	auto transactionBody = builder.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_TRUE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->getMemo(), memo);
	EXPECT_EQ(transactionBody->getCreatedAt(), Timestamp(now));

	auto transferAmount = transactionBody->getTransferAmount();
	if(!transferAmount) {
		printf("transfer amount is null\n");
	} else {
		EXPECT_TRUE(transferAmount);
		EXPECT_EQ(transferAmount->getAmount().toString(), "1000.0000");
	}

	auto creation = transactionBody->getCreation();
	auto recipient = creation->getRecipient();
	EXPECT_EQ(*recipient.getPubkey(), *g_KeyPairs[4].publicKey);
	EXPECT_EQ(recipient.getCommunityId(), "");
	EXPECT_EQ(recipient.getAmount().toString(), "1000.0000");
	EXPECT_EQ(creation->getTargetDate(), TimestampSeconds(1660953712));
}


TEST(TransactionBodyBuilderTest, GradidoTransfer) {
	auto now = system_clock::now();
	std::string memo("Danke fuer dein Sein!");
	TransactionBodyBuilder builder;
	builder
		.setTransactionTransfer(
			TransferAmount(
				g_KeyPairs[4].publicKey, // sender
				"100.251621",
				""
			), g_KeyPairs[5].publicKey // recipient
		)
		.setCreatedAt(now)
		.setMemo(memo)
	;
	auto transactionBody = builder.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_TRUE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->getMemo(), memo);
	EXPECT_EQ(transactionBody->getCreatedAt(), Timestamp(now));

	auto transfer = transactionBody->getTransfer();
	auto sender = transfer->getSender();
	EXPECT_EQ(*sender.getPubkey(), *g_KeyPairs[4].publicKey);
	EXPECT_EQ(sender.getCommunityId(), "");
	EXPECT_EQ(sender.getAmount().toString(), "100.2516");
	EXPECT_EQ(*transfer->getRecipient(), *g_KeyPairs[5].publicKey);
}

TEST(TransactionBodyBuilderTest, GradidoDeferredTransfer) {
	auto now = system_clock::now();
	std::string memo("Probiere doch auch mal Gradidos aus");
	TransactionBodyBuilder builder;
	builder
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(
					g_KeyPairs[4].publicKey, // sender
					"100.251621",
					""
				), g_KeyPairs[5].publicKey // recipient
			), now + days(5)
		)
		.setCreatedAt(now)
		.setMemo(memo)
	;
	auto transactionBody = builder.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_TRUE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->getMemo(), memo);
	EXPECT_EQ(transactionBody->getCreatedAt(), Timestamp(now));

	auto deferredTransfer = transactionBody->getDeferredTransfer();
	auto transfer = deferredTransfer->getTransfer();
	auto sender = transfer.getSender();
	EXPECT_EQ(*sender.getPubkey(), *g_KeyPairs[4].publicKey);
	EXPECT_EQ(sender.getCommunityId(), "");
	EXPECT_EQ(sender.getAmount().toString(), "100.2516");
	EXPECT_EQ(*transfer.getRecipient(), *g_KeyPairs[5].publicKey);
	EXPECT_EQ(deferredTransfer->getTimeout(), now + days(5));
}

