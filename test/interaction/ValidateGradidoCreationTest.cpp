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

constexpr auto memo = "Deine erste Schoepfung;)";


TEST(ValidateGradidoCreationTest, valid) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoCreationTest, invalidMemoEmpty) {
	GradidoTransactionBuilder builder;
	builder
		// not set memo => empty memo
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, invalidMemoToShort) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("hall")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	// to short
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, invalidMemoToBig) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(std::string(451, 'a')) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);

	// to big
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidAmountNegative) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// negative amount
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "-1000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidAmountZero) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// zero amount
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "0"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidAmountToBig) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// to big amount, only 1000 per month allowed
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "2000.00"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, InvalidCoinCommunityIdIdenticalToBlockchainCommunityId) {
	std::string communityId = "testGroup";
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			// coin community id is identical to blockchain community id to which transaction belong
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00", communityId),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, communityId), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, InvalidCoinCommunityId) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "1000.00", "<script>"),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE), validate::TransactionValidationInvalidInputException);
}

// TODO: invalid target date with both algos