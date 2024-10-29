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
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[4])
	;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*transaction);
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
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isRegisterAddress());
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
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isRegisterAddress());
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
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isRegisterAddress());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::WrongAddressTypeException);
}

TEST(ValidateRegisterAddressTest, UserAndAccountPublicKeySame) {
	EXPECT_THROW(RegisterAddress(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3]->getPublicKey(),
		nullptr,
		g_KeyPairs[3]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateRegisterAddressTest, NullptrPublicKey) {
	EXPECT_THROW(RegisterAddress(
		AddressType::COMMUNITY_HUMAN,
		1,
		nullptr,
		nullptr,
		nullptr
	), GradidoNodeInvalidDataException);
}

TEST(ValidateRegisterAddressTest, EmptyUserPublicKey) {
	EXPECT_THROW(RegisterAddress(
		AddressType::COMMUNITY_HUMAN,
		1,
		std::make_shared<memory::Block>(32),
		nullptr,
		g_KeyPairs[4]->getPublicKey()
	), GradidoNodeInvalidDataException);
}

TEST(ValidateRegisterAddressTest, EmptyAccountPublicKey) {
	EXPECT_THROW(RegisterAddress(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3]->getPublicKey(),
		nullptr,
		std::make_shared<memory::Block>(32)
	), GradidoNodeInvalidDataException);
}


TEST(ValidateRegisterAddressTest, InvalidUserPublicKey) {
	EXPECT_THROW(RegisterAddress(
		AddressType::COMMUNITY_HUMAN,
		1,
		std::make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18)),
		nullptr,
		g_KeyPairs[4]->getPublicKey()
	), Ed25519InvalidKeyException);
}

TEST(ValidateRegisterAddressTest, InvalidAccountPublicKey) {
	EXPECT_THROW(RegisterAddress(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3]->getPublicKey(),
		nullptr,
		std::make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18))
	), Ed25519InvalidKeyException);
}
