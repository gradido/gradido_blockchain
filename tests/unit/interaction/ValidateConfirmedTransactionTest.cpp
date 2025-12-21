#include "gtest/gtest.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
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
		make_unique<GradidoTransaction>(*deserializer.getGradidoTransaction()),
		confirmedAt,
		GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING,
		LedgerAnchor(defaultHieroTransactionId),
		{},
		BalanceDerivationType::EXTERN
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
		make_unique<GradidoTransaction>(*deserializer.getGradidoTransaction()),
		confirmedAt,
		"1",
		LedgerAnchor(defaultHieroTransactionId),
		{},
		BalanceDerivationType::EXTERN
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
		make_unique<GradidoTransaction>(*deserializer.getGradidoTransaction()),
		confirmedAt,
		GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING,
		LedgerAnchor(),
		{},
		BalanceDerivationType::EXTERN
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

/* rule disabled for the time, because hedera move the confirmation date before the creation date
TEST(ValidateConfirmedTransactionTest, invalidConfirmedBeforeCreated) {
	auto communityRootRaw = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	interaction::deserialize::Context deserializer(communityRootRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());

	ConfirmedTransaction confirmedTransaction(
		7,
		make_unique<GradidoTransaction>(*deserializer.getGradidoTransaction()),
		createdAt - chrono::seconds{ 1 },
		GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING,
		LedgerAnchor(defaultHieroTransactionId),
		{},
		BalanceDerivationType::EXTERN
	);

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}
*/