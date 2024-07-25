#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;


TEST(ValidateGradidoTransferTest, Valid) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransferTest, invalidMemo) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
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

TEST(ValidateGradidoTransferTest, InvalidAmount) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "0"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "-100.00"),
		g_KeyPairs[5].publicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidCoinCommunityId) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	// coin community id is identical to blockchain community id to which transaction belong
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "0", "testGroup"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, "testGroup"), validate::TransactionValidationInvalidInputException);

	// invalid coin community id
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "0", "<script>"),
		g_KeyPairs[5].publicKey
	);
	EXPECT_THROW(c.run(validate::Type::SINGLE, "testGroup"), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, SenderAndRecipientIdentical) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateGradidoTransferTest, NullptrPublicKey) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(nullptr, "500.55"),
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		nullptr
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, EmptyPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(emptyPublicKey, "500.55"),
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		emptyPublicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(invalidPublicKey, "500.55"),
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		invalidPublicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}