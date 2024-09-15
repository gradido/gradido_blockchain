#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

constexpr auto memo = "Ich teile mit dir";

TEST(ValidateGradidoTransferTest, Valid) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransferTest, Outbound) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity("dummy-group")
		.setRecipientCommunity("gratitude")
		;
	auto body = builder.buildOutbound();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransferTest, invalidMemoEmpty) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, invalidMemoToShort) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("hall")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	// to short
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, invalidMemoToBig) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(std::string(451, 'a')) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);

	// to big
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}



TEST(ValidateGradidoTransferTest, InvalidAmountZero) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "0"), // zero amount
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidAmountNegative) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "-100.00"), // negative amount
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, InvalidCoinCommunityIdIdenticalToBlockchainCommunityId) {
	std::string communityId = "testGroup";
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			// coin community id is identical to blockchain community id to which transaction belong
			// not needed so it is a error
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55", communityId),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, communityId), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidCoinCommunityId) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			// invalid character in coin community id
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55", "<script>"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, SenderAndRecipientIdentical) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			g_KeyPairs[4]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateGradidoTransferTest, NullptrSenderPublicKey) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(nullptr, "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, NullptrRecipientPublicKey) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			nullptr
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, EmptySenderPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(emptyPublicKey, "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, EmptyRecipientPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			emptyPublicKey
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidSenderPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(invalidPublicKey, "500.55"),
			g_KeyPairs[5]->getPublicKey()
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidRecipientPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
			invalidPublicKey
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->getTransactionBody()->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}
