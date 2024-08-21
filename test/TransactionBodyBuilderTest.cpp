
#include "gtest/gtest.h"
#include "KeyPairs.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"

using namespace gradido;
using namespace gradido::data;
using namespace std::chrono;


TEST(TransactionBodyBuilderTest, CommunityRoot) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setCommunityRoot(g_KeyPairs[0].publicKey, g_KeyPairs[1].publicKey, g_KeyPairs[2].publicKey)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_TRUE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	auto communityRoot = transactionBody->communityRoot;
	
	EXPECT_EQ(*communityRoot->pubkey, *g_KeyPairs[0].publicKey);
	EXPECT_EQ(*communityRoot->gmwPubkey, *g_KeyPairs[1].publicKey);
	EXPECT_EQ(*communityRoot->aufPubkey, *g_KeyPairs[2].publicKey);
	EXPECT_GT(transactionBody->createdAt, Timestamp(now));
}

TEST(TransactionBodyBuilderTest, RegisterAddressUserOnly) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setRegisterAddress(g_KeyPairs[3].publicKey, AddressType::COMMUNITY_HUMAN)
		.setCreatedAt(now)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_TRUE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->memo, "");
	EXPECT_EQ(transactionBody->createdAt, Timestamp(now));

	auto registerAddress = transactionBody->registerAddress;
	EXPECT_EQ(registerAddress->addressType, AddressType::COMMUNITY_HUMAN);
	EXPECT_EQ(*registerAddress->userPubkey, *g_KeyPairs[3].publicKey);

}

TEST(TransactionBodyBuilderTest, RegisterAddressUserAndAccount) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr, 
			g_KeyPairs[4].publicKey
		)
		.setCreatedAt(now)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_TRUE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->memo, "");
	EXPECT_EQ(transactionBody->createdAt, Timestamp(now));

	auto registerAddress = transactionBody->registerAddress;
	EXPECT_EQ(registerAddress->addressType, AddressType::COMMUNITY_HUMAN);
	EXPECT_EQ(*registerAddress->userPubkey, *g_KeyPairs[3].publicKey);
	EXPECT_EQ(*registerAddress->accountPubkey, *g_KeyPairs[4].publicKey);
}

TEST(TransactionBodyBuilderTest, GradidoCreation) {
	auto now = system_clock::now();
	std::string memo("Du bist der Schoepfer deiner Welt!");
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setTransactionCreation(
			g_KeyPairs[4].publicKey,
			"1000",
			TimestampSeconds(1660953712)
		)
		.setCreatedAt(now)
		.setMemo(memo)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_TRUE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->memo, memo);
	EXPECT_EQ(transactionBody->createdAt, Timestamp(now));

	auto creation = transactionBody->creation;
	EXPECT_EQ(*creation->recipient.mPubkey, *g_KeyPairs[4].publicKey);
	EXPECT_EQ(creation->recipient.mCommunityId, "");
	EXPECT_EQ(creation->recipient.mAmount.toString(), "1000.0000");
	EXPECT_EQ(creation->targetDate, TimestampSeconds(1660953712));
}


TEST(TransactionBodyBuilderTest, GradidoTransfer) {
	auto now = system_clock::now();
	std::string memo("Danke fuer dein Sein!");
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setTransactionTransfer(
			g_KeyPairs[4].publicKey, // sender
			"100.251621",
			"",
			g_KeyPairs[5].publicKey // recipient
		)
		.setCreatedAt(now)
		.setMemo(memo)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_TRUE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->memo, memo);
	EXPECT_EQ(transactionBody->createdAt, Timestamp(now));

	auto transfer = transactionBody->transfer;
	EXPECT_EQ(*transfer->sender.mPubkey, *g_KeyPairs[4].publicKey);
	EXPECT_EQ(transfer->sender.mCommunityId, "");
	EXPECT_EQ(transfer->sender.mAmount.toString(), "100.2516");
	EXPECT_EQ(*transfer->recipient, *g_KeyPairs[5].publicKey);
}

TEST(TransactionBodyBuilderTest, GradidoDeferredTransfer) {
	auto now = system_clock::now();
	std::string memo("Probiere doch auch mal Gradidos aus");
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setDeferredTransfer(
			g_KeyPairs[4].publicKey, // sender
			"100.251621",
			"",
			g_KeyPairs[5].publicKey, // recipient
			now + days(5)
		)
		.setCreatedAt(now)
		.setMemo(memo)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_FALSE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_TRUE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->memo, memo);
	EXPECT_EQ(transactionBody->createdAt, Timestamp(now));

	auto deferredTransfer = transactionBody->deferredTransfer;
	EXPECT_EQ(*deferredTransfer->transfer.sender.mPubkey, *g_KeyPairs[4].publicKey);
	EXPECT_EQ(deferredTransfer->transfer.sender.mCommunityId, "");
	EXPECT_EQ(deferredTransfer->transfer.sender.mAmount.toString(), "100.2516");
	EXPECT_EQ(*deferredTransfer->transfer.recipient, *g_KeyPairs[5].publicKey);
	EXPECT_EQ(deferredTransfer->timeout, now + days(5));
}

