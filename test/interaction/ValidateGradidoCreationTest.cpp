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

constexpr auto memo = "Deine erste Schoepfung;)";


TEST(ValidateGradidoCreationTest, valid) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoCreationTest, invalidMemoEmpty) {
	TransactionBodyBuilder builder;
	builder
		// not set memo => empty memo
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, invalidMemoToShort) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("hall")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	// to short
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, invalidMemoToBig) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(std::string(451, 'a')) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);

	// to big
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidAmountNegative) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// negative amount
			TransferAmount(g_KeyPairs[4].publicKey, "-1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidAmountZero) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// zero amount
			TransferAmount(g_KeyPairs[4].publicKey, "0"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidAmountToBig) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// to big amount, only 1000 per month allowed
			TransferAmount(g_KeyPairs[4].publicKey, "2000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, InvalidCoinCommunityIdIdenticalToBlockchainCommunityId) {
	std::string communityId = "testGroup";
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// coin community id is identical to blockchain community id to which transaction belong
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00", communityId),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, communityId), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, InvalidCoinCommunityId) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4].publicKey, "1000.00", "<script>"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isCreation());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, NullptrRecipientPublicKey) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(nullptr, "1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, EmptyRecipientPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(emptyPublicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, InvalidRecipientPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(18);
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(invalidPublicKey, "1000.00"),
			TimestampSeconds(1609459000)
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

// TODO: invalid target date with both algos