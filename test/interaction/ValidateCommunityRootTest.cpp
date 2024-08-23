#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateCommunityRootTest, Valid)
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
	validate::Context c(*body);
	// body only
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateCommunityRootTest, InvalidGMWAndAUFSame)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey,
			g_KeyPairs[1].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndAUFSame)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[1].publicKey,
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndGMWSame)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeysAllSame)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[0].publicKey,
			g_KeyPairs[0].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, EmptyPublicKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			make_shared<memory::Block>(32),
			g_KeyPairs[1].publicKey,
			g_KeyPairs[2].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, EmptyGMWKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			make_shared<memory::Block>(32),
			g_KeyPairs[2].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, EmptyAUFKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey,
			make_shared<memory::Block>(32)
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateCommunityRootTest, InvalidPublicKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a")),
			g_KeyPairs[1].publicKey,
			g_KeyPairs[2].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, InvalidGMWKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a")),
			g_KeyPairs[2].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, InvalidAUFKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey,
			make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a"))
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateCommunityRootTest, NullptrPublicKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			nullptr,
			g_KeyPairs[1].publicKey,
			g_KeyPairs[2].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, NullptrGMWKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			nullptr,
			g_KeyPairs[2].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, NullptrAUFKey)
{
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0].publicKey,
			g_KeyPairs[1].publicKey,
			nullptr
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}