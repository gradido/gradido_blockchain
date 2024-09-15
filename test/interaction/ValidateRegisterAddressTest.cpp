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

TEST(ValidateRegisterAddressTest, Valid) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr, 
			g_KeyPairs[4]->getPublicKey()
		)
	;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateRegisterAddressTest, InvalidAddressTypeGMW) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_GMW,
			nullptr,
			g_KeyPairs[4]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, InvalidAddressTypeAUF) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_AUF,
			nullptr,
			g_KeyPairs[4]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, InvalidAddressTypeNONE) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::NONE,
			nullptr,
			g_KeyPairs[4]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, UserAndAccountPublicKeySame) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[3]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateRegisterAddressTest, NullptrPublicKey) {
	GradidoTransactionBuilder builder;
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
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateRegisterAddressTest, EmptyUserPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			emptyPublicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[4]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateRegisterAddressTest, EmptyAccountPublicKey) {
	auto emptyPublicKey = std::make_shared<memory::Block>(32);
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			emptyPublicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateRegisterAddressTest, InvalidUserPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			invalidPublicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[4]->getPublicKey()
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateRegisterAddressTest, InvalidAccountPublicKey) {
	auto invalidPublicKey = std::make_shared<memory::Block>(10);
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[3]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			invalidPublicKey
		)
		;
	auto body = builder.build();
	ASSERT_TRUE(body->getTransactionBody()->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}
