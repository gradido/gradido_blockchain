#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;


TEST(ValidateGradidoCreationTest, valid) {
	TransactionBody body("Deine erste Schoepfung ;)", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
	validate::Context c(body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoCreationTest, invalidMemo) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
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

TEST(ValidateGradidoCreationTest, invalidAmount) {
	TransactionBody body("Deine erste Schoepfung ;)", createdAt, VERSION_STRING);
	// negative amount
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "-1000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	// zero amount
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "0.00"),
		TimestampSeconds(1609459000)
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	// to big amount
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "2000.00"),
		TimestampSeconds(1609459000)
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
	
}

TEST(ValidateGradidoCreationTest, InvalidCoinCommunityId) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	// coin community id is identical to blockchain community id to which transaction belong
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "-1000.00", "testGroup"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
	validate::Context c(body);
	EXPECT_THROW(c.run(validate::Type::SINGLE, "testGroup"), validate::TransactionValidationInvalidInputException);

	// invalid coin community id
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "0", "<script>"),
		TimestampSeconds(1609459000)
	);
	EXPECT_THROW(c.run(validate::Type::SINGLE, "testGroup"), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoCreationTest, invalidPubkey) {
	TransactionBody body("Deine erste Schoepfung ;)", createdAt, VERSION_STRING);
	// nullptr key
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(nullptr, "2000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
	validate::Context c(body);

	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	// empty key
	auto publicKey = std::make_shared<memory::Block>(32);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(publicKey, "2000.00"),
		TimestampSeconds(1609459000)
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	// invalid key
	auto invalidKey = std::make_shared<memory::Block>(18);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(invalidKey, "2000.00"),
		TimestampSeconds(1609459000)
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

