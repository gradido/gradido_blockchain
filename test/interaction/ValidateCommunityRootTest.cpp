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
	validate::Context c(*body);
	// body only
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateCommunityRootTest, InvalidGMWAndAUFSame)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndAUFSame)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndGMWSame)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeysAllSame)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[0]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, EmptyPublicKey)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			make_shared<memory::Block>(32),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, EmptyGMWKey)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			make_shared<memory::Block>(32),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, EmptyAUFKey)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			make_shared<memory::Block>(32)
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateCommunityRootTest, InvalidPublicKey)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a")),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, InvalidGMWKey)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a")),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, InvalidAUFKey)
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a"))
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCommunityRoot());
	validate::Context c(*body);
	// body only
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateCommunityRootTest, NullptrPublicKey)
{
	GradidoTransactionBuilder builder;
	EXPECT_THROW(builder.setCommunityRoot(
		nullptr,
		g_KeyPairs[1]->getPublicKey(),
		g_KeyPairs[2]->getPublicKey()
	), GradidoNullPointerException);
}

TEST(ValidateCommunityRootTest, NullptrGMWKey)
{
	GradidoTransactionBuilder builder;
	EXPECT_THROW(builder.setCommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		nullptr,
		g_KeyPairs[2]->getPublicKey()
	), GradidoNullPointerException);
}

TEST(ValidateCommunityRootTest, NullptrAUFKey)
{
	GradidoTransactionBuilder builder;
	EXPECT_THROW(builder.setCommunityRoot(
		g_KeyPairs[0]->getPublicKey(),
		g_KeyPairs[1]->getPublicKey(),
		nullptr
	), GradidoNullPointerException);
}