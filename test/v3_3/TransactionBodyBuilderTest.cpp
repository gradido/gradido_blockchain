#include "TransactionBodyBuilderTest.h"
#include "gradido_blockchain/v3_3/TransactionBodyBuilder.h"

using namespace gradido::v3_3;
using namespace gradido::v3_3::data;
using namespace std::chrono;

void TransactionBodyBuilderTest::SetUp()
{
	unsigned char hash[crypto_hash_sha512_BYTES];
	std::array seeds = {
		"Ein Tauschmittel fuer die Menschen",
		"Liebe und Gesundheit",
		"hurra auf das dreifache Wohl",
		"Wir machen die Erde zu einem Paradies fuer alle Wesen",
		"Dankesausgleich",
		"Folgt seinem Ruf"
	};
	mED25519KeyPairs.reserve(seeds.size());
	for (int i = 0; i < seeds.size(); i++) {
		KeyPair keyPair;
		auto& seed = seeds[i];
		
		crypto_hash_sha512(hash, (const unsigned char*)seed, strlen(seed));
		crypto_sign_seed_keypair(*keyPair.publicKey, *keyPair.privateKey, hash);
		mED25519KeyPairs.push_back(keyPair);
	}
}

void TransactionBodyBuilderTest::TearDown()
{

}

TEST_F(TransactionBodyBuilderTest, CommunityRoot) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	std::string memo("Die Welt und ein Paar Schlittschuhe dazu.");
	auto transactionBody = builder
		.setCommunityRoot(mED25519KeyPairs[0].publicKey, mED25519KeyPairs[1].publicKey, mED25519KeyPairs[2].publicKey)
		.setMemo(memo)
		.build();

	EXPECT_FALSE(transactionBody->isCommunityFriendsUpdate());
	EXPECT_TRUE(transactionBody->isCommunityRoot());
	EXPECT_FALSE(transactionBody->isCreation());
	EXPECT_FALSE(transactionBody->isDeferredTransfer());
	EXPECT_FALSE(transactionBody->isRegisterAddress());
	EXPECT_FALSE(transactionBody->isTransfer());

	EXPECT_EQ(transactionBody->memo, memo);

	auto communityRoot = transactionBody->communityRoot;
	
	EXPECT_EQ(*communityRoot->pubkey, *mED25519KeyPairs[0].publicKey);
	EXPECT_EQ(*communityRoot->gmwPubkey, *mED25519KeyPairs[1].publicKey);
	EXPECT_EQ(*communityRoot->aufPubkey, *mED25519KeyPairs[2].publicKey);
	EXPECT_GT((Timepoint)transactionBody->createdAt, now);
}

TEST_F(TransactionBodyBuilderTest, RegisterAddressUserOnly) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setRegisterAddress(mED25519KeyPairs[3].publicKey, AddressType::COMMUNITY_HUMAN)
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
	EXPECT_EQ(*registerAddress->userPubkey, *mED25519KeyPairs[3].publicKey);

}

TEST_F(TransactionBodyBuilderTest, RegisterAddressUserAndAccount) {
	auto now = system_clock::now();
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setRegisterAddress(
			mED25519KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr, 
			mED25519KeyPairs[4].publicKey
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
	EXPECT_EQ(*registerAddress->userPubkey, *mED25519KeyPairs[3].publicKey);
	EXPECT_EQ(*registerAddress->accountPubkey, *mED25519KeyPairs[4].publicKey);
}

TEST_F(TransactionBodyBuilderTest, GradidoCreation) {
	auto now = system_clock::now();
	std::string memo("Du bist der Schoepfer deiner Welt!");
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setTransactionCreation(
			mED25519KeyPairs[4].publicKey,
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
	EXPECT_EQ(*creation->recipient.pubkey, *mED25519KeyPairs[4].publicKey);
	EXPECT_EQ(creation->recipient.communityId, "");
	EXPECT_EQ(creation->recipient.amount.toString(2), "1000.00");
	EXPECT_EQ(creation->targetDate, TimestampSeconds(1660953712));
}


TEST_F(TransactionBodyBuilderTest, GradidoTransfer) {
	auto now = system_clock::now();
	std::string memo("Danke fuer dein Sein!");
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setTransactionTransfer(
			mED25519KeyPairs[4].publicKey, // sender
			"100.251621",
			"",
			mED25519KeyPairs[5].publicKey // recipient
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
	EXPECT_EQ(*transfer->sender.pubkey, *mED25519KeyPairs[4].publicKey);
	EXPECT_EQ(transfer->sender.communityId, "");
	EXPECT_EQ(transfer->sender.amount.toString(6), "100.251621");
	EXPECT_EQ(*transfer->recipient, *mED25519KeyPairs[5].publicKey);
}

TEST_F(TransactionBodyBuilderTest, GradidoDeferredTransfer) {
	auto now = system_clock::now();
	std::string memo("Danke fuer dein Sein!");
	TransactionBodyBuilder builder;
	auto transactionBody = builder
		.setDeferredTransfer(
			mED25519KeyPairs[4].publicKey, // sender
			"100.251621",
			"",
			mED25519KeyPairs[5].publicKey, // recipient
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
	EXPECT_EQ(*deferredTransfer->transfer.sender.pubkey, *mED25519KeyPairs[4].publicKey);
	EXPECT_EQ(deferredTransfer->transfer.sender.communityId, "");
	EXPECT_EQ(deferredTransfer->transfer.sender.amount.toString(6), "100.251621");
	EXPECT_EQ(*deferredTransfer->transfer.recipient, *mED25519KeyPairs[5].publicKey);
	EXPECT_EQ(deferredTransfer->timeout, now + days(5));
}