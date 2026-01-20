#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateGradidoCreationTest, valid) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.addMemo(hallMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.addMemo(aFilledMemoString) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			// negative amount
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000).negated(), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			// zero amount
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::zero(), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
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
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			// to big amount, only 1000 per month allowed
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(20000000), 0),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isCreation());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoCreationTest, InvalidCoinCommunityId) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(creationMemoString)
		.setCreatedAt(createdAt)
		.setVersionNumber(GRADIDO_TRANSACTION_BODY_VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(10000000), 1),
			TimestampSeconds(1609459000)
		)
		.setRecipientCommunity(communityId);

	EXPECT_THROW(builder.sign(g_KeyPairs[6]), DictionaryMissingEntryException);
}

// TODO: invalid target date with both algos