#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateRegisterAddressTest, Valid) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());
	validate::Context c(body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateRegisterAddressTest, InvalidAddressType) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_GMW,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);

	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_AUF,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);

	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::NONE,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, UserAndAccountPublicKeySame) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[3].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateRegisterAddressTest, NullptrPublicKey) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		nullptr,
		nullptr,
		nullptr
	);
	ASSERT_TRUE(body.isRegisterAddress());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateRegisterAddressTest, EmptyPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		emptyPublicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		emptyPublicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateRegisterAddressTest, InvalidPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		invalidPublicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);

	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		invalidPublicKey
	);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}