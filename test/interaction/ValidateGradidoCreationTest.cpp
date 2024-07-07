#include "gtest/gtest.h"
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