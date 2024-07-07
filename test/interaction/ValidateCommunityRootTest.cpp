#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;


TEST(ValidateCommunityRootTest, Valid)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateCommunityRootTest, InvalidGMWAndAUFSame)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[1].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndAUFSame)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[1].publicKey,
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeyAndGMWSame)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateCommunityRootTest, InvalidPublicKeysAllSame)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[0].publicKey,
		g_KeyPairs[0].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}


TEST(ValidateCommunityRootTest, InvalidEmptyPublic)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	memory::BlockPtr emptyPublicKey = make_shared<memory::Block>(32);
	body.communityRoot = make_shared<CommunityRoot>(
		emptyPublicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		emptyPublicKey,
		g_KeyPairs[2].publicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		emptyPublicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateCommunityRootTest, InvalidInvalidPublic)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	memory::BlockPtr invalidPublicKey = make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a"));
	body.communityRoot = make_shared<CommunityRoot>(
		invalidPublicKey,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		invalidPublicKey,
		g_KeyPairs[2].publicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		invalidPublicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateCommunityRootTest, InvalidNullptrPublic)
{
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityRoot = make_shared<CommunityRoot>(
		nullptr,
		g_KeyPairs[1].publicKey,
		g_KeyPairs[2].publicKey
	);
	ASSERT_TRUE(body.isCommunityRoot());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		nullptr,
		g_KeyPairs[2].publicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.communityRoot = make_shared<CommunityRoot>(
		g_KeyPairs[0].publicKey,
		g_KeyPairs[1].publicKey,
		nullptr
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

