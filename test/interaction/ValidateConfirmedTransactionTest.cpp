#include "gtest/gtest.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
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
	auto communityRootRaw = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	interaction::deserialize::Context deserializer(communityRootRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());

	ConfirmedTransaction confirmedTransaction(
		7,
		deserializer.getGradidoTransaction(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(memory::Block::fromBase64(hieroTransactionIdBase64)),
		{}
	);

	validate::Context c(confirmedTransaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateConfirmedTransactionTest, invalidWrongVersion) {
	auto communityRootRaw = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	interaction::deserialize::Context deserializer(communityRootRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());

	ConfirmedTransaction confirmedTransaction(
		7,
		deserializer.getGradidoTransaction(),
		confirmedAt,
		"1",
		make_shared<memory::Block>(32),
		{}
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateConfirmedTransactionTest, invalidMessageID) {
	auto communityRootRaw = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	interaction::deserialize::Context deserializer(communityRootRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());

	ConfirmedTransaction confirmedTransaction(
		7,
		deserializer.getGradidoTransaction(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(10),
		{}
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateConfirmedTransactionTest, invalidConfirmedBeforeCreated) {
	auto communityRootRaw = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	interaction::deserialize::Context deserializer(communityRootRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());

	ConfirmedTransaction confirmedTransaction(
		7,
		deserializer.getGradidoTransaction(),
		createdAt - chrono::seconds{ 1 },
		VERSION_STRING,
		make_shared<memory::Block>(32),
		{}
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}
