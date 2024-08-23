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

TEST(ValidateRegisterAddressTest, Valid) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr, 
			g_KeyPairs[4].publicKey
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateRegisterAddressTest, InvalidAddressTypeGMW) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_GMW,
			nullptr,
			g_KeyPairs[4].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, InvalidAddressTypeAUF) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_AUF,
			nullptr,
			g_KeyPairs[4].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, InvalidAddressTypeNONE) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::NONE,
			nullptr,
			g_KeyPairs[4].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, UserAndAccountPublicKeySame) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[3].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateRegisterAddressTest, NullptrPublicKey) {
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			nullptr,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			nullptr
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateRegisterAddressTest, EmptyUserPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			emptyPublicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[4].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateRegisterAddressTest, EmptyAccountPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			emptyPublicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateRegisterAddressTest, InvalidUserPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			invalidPublicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[4].publicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateRegisterAddressTest, InvalidAccountPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	TransactionBodyBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			invalidPublicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}
