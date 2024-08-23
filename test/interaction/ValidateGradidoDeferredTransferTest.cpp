#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

// createdAt: 1609459200
// timeout:	  1609465000
constexpr auto memo = "Link zum einloesen";

TEST(ValidateGradidoDeferredTransferTest, Valid) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemoEmpty) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemoToShort) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("hall")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	// to short
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemoToBig) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(std::string(451, 'a')) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);

	// to big
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, InvalidAmountZero) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo) 
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "0"), // zero amount
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidAmountNegative) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "-100.00"), // negative amount
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidCoinCommunityIdIdenticalToBlockchainCommunityId) {
	std::string communityId = "testGroup";
	TransactionBodyBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				// coin community id is identical to blockchain community id to which transaction belong
				// not needed so it is a error
				TransferAmount(g_KeyPairs[4].publicKey, "500.55", communityId),
				g_KeyPairs[5].publicKey
			), timeout
		)
	;
	auto body = builder.build();	
	ASSERT_TRUE(body->isDeferredTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, communityId), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidCoinCommunityId) {
	TransactionBodyBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				// invalid character in coin community id
				TransferAmount(g_KeyPairs[4].publicKey, "500.55", "<script>"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isDeferredTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, SenderAndRecipientIdentical) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"), 
				g_KeyPairs[4].publicKey
			), timeout
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isDeferredTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateGradidoDeferredTransferTest, NullptrSenderPublicKey) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(nullptr, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, NullptrRecipientPublicKey) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				nullptr
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, EmptySenderPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(emptyPublicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, EmptyRecipientPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				emptyPublicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidSenderPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(invalidPublicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidRecipientPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				invalidPublicKey
			), timeout
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, InvalidTimeoutAboveMaxHardLimit) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), createdAt + chrono::seconds{ 7962400 }
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidTimeoutIdenticalToCreatedAt) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), createdAt
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidTimeoutBeforeCreatedAt) {
	TransactionBodyBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
				g_KeyPairs[5].publicKey
			), createdAt - chrono::seconds{ 1 }
		)
		;
	auto body = builder.build();

	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}