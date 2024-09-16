#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
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
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemoEmpty) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemoToShort) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("hall")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	// to short
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemoToBig) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(std::string(451, 'a')) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);

	// to big
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoDeferredTransferTest, InvalidAmountZero) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo) 
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "0"), // zero amount
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidAmountNegative) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "-100.00"), // negative amount
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidCoinCommunityIdIdenticalToBlockchainCommunityId) {
	std::string communityId = "testGroup";
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				// coin community id is identical to blockchain community id to which transaction belong
				// not needed so it is a error
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55", communityId),
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, communityId), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidCoinCommunityId) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo("Ich teile mit dir")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				// invalid character in coin community id
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55", "<script>"),
				g_KeyPairs[5]->getPublicKey()
			), timeout
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(validate::Type::SINGLE), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidTimeoutAboveMaxHardLimit) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), createdAt + chrono::seconds{ 7962400 }
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidTimeoutIdenticalToCreatedAt) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), createdAt
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidTimeoutBeforeCreatedAt) {
	GradidoTransactionBuilder builder;
	builder
		.setMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), "500.55"),
				g_KeyPairs[5]->getPublicKey()
			), createdAt - chrono::seconds{ 1 }
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isDeferredTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}