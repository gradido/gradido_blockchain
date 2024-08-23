#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateConfirmedTransactionTest, validCommunityRootGradidoTransaction) {
	// keyPair 0 is public of this community root Transaction
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
	;

	ConfirmedTransaction confirmedTransaction(
		7,
		builder.build(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(32),
		"899.748379"
	);

	validate::Context c(confirmedTransaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateConfirmedTransactionTest, invalidWrongVersion) {
	// keyPair 0 is public of this community root Transaction
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
		;

	ConfirmedTransaction confirmedTransaction(
		7,
		builder.build(),
		confirmedAt,
		"1",
		make_shared<memory::Block>(32),
		"899.748379"
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateConfirmedTransactionTest, invalidMessageID) {
	// keyPair 0 is public of this community root Transaction
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
		;

	ConfirmedTransaction confirmedTransaction(
		7,
		builder.build(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(10),
		"899.748379"
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateConfirmedTransactionTest, invalidConfirmedBeforeCreated) {
	// keyPair 0 is public of this community root Transaction
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
		;

	ConfirmedTransaction confirmedTransaction(
		7,
		builder.build(),
		createdAt - chrono::seconds{ 1 },
		VERSION_STRING,
		make_shared<memory::Block>(32),
		"899.748379"
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}
