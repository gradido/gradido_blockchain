#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateCommunityRootTest, Valid)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey()->data(),
			g_KeyPairs[1]->getPublicKey()->data(),
			g_KeyPairs[2]->getPublicKey()->data()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[0])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateCommunityRootTest, InvalidGMWAndAUFSame)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndAUFSame)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[1]->getPublicKey(),
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndGMWSame)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeysAllSame)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[0]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateCommunityRootTest, EmptyPublicKey)
{
	EXPECT_THROW(CommunityRoot(
		make_shared<memory::Block>(32),
		g_KeyPairs[1]->getPublicKey(),
		g_KeyPairs[2]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateCommunityRootTest, EmptyGMWKey)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		make_shared<memory::Block>(32),
		g_KeyPairs[2]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateCommunityRootTest, EmptyAUFKey)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey(),
		make_shared<memory::Block>(32)
	), GradidoNodeInvalidDataException);
}


TEST(ValidateCommunityRootTest, InvalidPublicKey)
{
	EXPECT_THROW(CommunityRoot(
		make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18)),
		g_KeyPairs[1]->getPublicKey(),
		g_KeyPairs[2]->getPublicKey()
	), Ed25519InvalidKeyException);
}

TEST(ValidateCommunityRootTest, InvalidGMWKey)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18)),
		g_KeyPairs[2]->getPublicKey()
	), Ed25519InvalidKeyException);
}

TEST(ValidateCommunityRootTest, InvalidAUFKey)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey(),
		make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18))
	), Ed25519InvalidKeyException);
}


TEST(ValidateCommunityRootTest, NullptrPublicKey)
{
	EXPECT_THROW(CommunityRoot(
		nullptr,
		g_KeyPairs[1]->getPublicKey(),
		g_KeyPairs[2]->getPublicKey()
	), GradidoNullPointerException);
}

TEST(ValidateCommunityRootTest, NullptrGMWKey)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		nullptr,
		g_KeyPairs[2]->getPublicKey()
	), GradidoNullPointerException);
}

TEST(ValidateCommunityRootTest, NullptrAUFKey)
{
	EXPECT_THROW(CommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey(),
		nullptr
	), GradidoNullPointerException);
}