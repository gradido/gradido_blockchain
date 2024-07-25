#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
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
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[5].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_NO_THROW(c.run());
}


TEST(ValidateGradidoDeferredTransferTest, invalidMemo) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[5].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	// to short
	body.memo = "hall";
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	// to big
	// fill with 451 x a
	body.memo = std::string(451, 'a');
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

}

TEST(ValidateGradidoDeferredTransferTest, InvalidAmount) {
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "0"),
			g_KeyPairs[5].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "-100.00"),
			g_KeyPairs[5].publicKey
		),
		timeout
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidCoinCommunityId) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	// coin community id is identical to blockchain community id to which transaction belong
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "0", "testGroup"),
			g_KeyPairs[5].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, "testGroup"), validate::TransactionValidationInvalidInputException);

	// invalid coin community id
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "0", "<script>"),
			g_KeyPairs[5].publicKey
		),
		timeout
	);
	EXPECT_THROW(c.run(validate::Type::SINGLE, "testGroup"), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, SenderAndRecipientIdentical) {
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[4].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateGradidoDeferredTransferTest, NullptrPublicKey) {
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(nullptr, "500.55"),
			g_KeyPairs[4].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			nullptr
		),
		timeout
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, EmptyPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(emptyPublicKey, "500.55"),
			g_KeyPairs[4].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			emptyPublicKey
		),
		timeout
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(invalidPublicKey, "500.55"),
			g_KeyPairs[4].publicKey
		),
		timeout
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			invalidPublicKey
		),
		timeout
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoDeferredTransferTest, InvalidTimeout) {
	TransactionBody body(memo, createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[5].publicKey
		),
		createdAt + chrono::seconds{ 7962400 }
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[5].publicKey
		),
		createdAt
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
			g_KeyPairs[5].publicKey
		),
		createdAt - chrono::seconds{1}
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}