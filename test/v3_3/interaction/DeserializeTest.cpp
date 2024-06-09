#include "gtest/gtest.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"
#include "../../KeyPairs.h"

using namespace gradido::v3_3;
using namespace data;
using namespace interaction;

#define VERSION_STRING "3.3"
const auto createdAt = std::chrono::system_clock::from_time_t(1609459200); //2021-01-01 00:00:00 UTC
const auto confirmedAt = std::chrono::system_clock::from_time_t(1609464130);

TEST(DeserializeTest, TransactionBodyWithoutMemo)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64("CgASCAiAzLn/BRAAGgMzLjMgAA=="));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());
}


TEST(DeserializeTest, CommunityRootBody)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hogUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	ASSERT_TRUE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto communityRoot = body->communityRoot;
	EXPECT_TRUE(communityRoot->pubkey->isTheSame(g_KeyPairs[0].publicKey));
	EXPECT_TRUE(communityRoot->gmwPubkey->isTheSame(g_KeyPairs[1].publicKey));
	EXPECT_TRUE(communityRoot->aufPubkey->isTheSame(g_KeyPairs[2].publicKey));
}
