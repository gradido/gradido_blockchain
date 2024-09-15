#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "const.h"
#include "../serializedTransactions.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;
using namespace memory;


TEST(ValidateGradidoTransaction, invalidBody) {
	constexpr auto invalidBodyString =
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson ";

	GradidoTransactionBuilder builder;	
	auto transaction = builder
		.setTransactionBody(make_shared<Block>(invalidBodyString))
		.build()
	;
	validate::Context c(*transaction);
	EXPECT_THROW(c.run(), GradidoNullPointerException);
}

TEST(ValidateGradidoTransaction, validCommunityRootGradidoTransaction) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(communityRootTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[0])
		.build()
		;
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidCommunityRootNotSigned) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(communityRootTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.build()
		;
	validate::Context c(*transaction);
	// no signature at all
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

TEST(ValidateGradidoTransaction, invalidCommunityRootWrongSigner) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(communityRootTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[1])
		.build()
		;
	validate::Context c(*transaction);
	// no signature at all
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}

TEST(ValidateGradidoTransaction, validRegisterAddressTransaction) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(registeAddressTransactionBase64));

	// must be signed two times, with user or account pubkey and with community root pubkey
	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[4])
		.build()
		;
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidRegisterAddressTransactionWithoutSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(registeAddressTransactionBase64));

	// must be signed two times, with user or account pubkey and with community root pubkey
	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.build()
		;
	validate::Context c(*transaction);
	// missing signature
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

TEST(ValidateGradidoTransaction, invalidRegisterAddressTransactionMissingSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(registeAddressTransactionBase64));

	// must be signed two times, with user or account pubkey and with community root pubkey
	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[0])
		.build()
		;
	validate::Context c(*transaction);
	// still missing signature
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

TEST(ValidateGradidoTransaction, invalidRegisterAddressTransactionMissingRequiredSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(registeAddressTransactionBase64));

	// must be signed two times, with user or account pubkey and with community root pubkey
	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[0])
		.sign(g_KeyPairs[3])
		.build()
		;
	validate::Context c(*transaction);
	// account public key set but signed with user pubkey, so sign with account pubkey is still missing
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}


TEST(ValidateGradidoTransaction, validGradidoCreationTransaction) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(creationTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[3])
		.build()
		;
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidGradidoCreationTransactionMissingSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(creationTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.build()
		;
	validate::Context c(*transaction);
	// no signature at all
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

// creation transaction must be signed by another user
TEST(ValidateGradidoTransaction, invalidGradidoCreationTransactionWrongSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(creationTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[4])
		.build()
		;
	validate::Context c(*transaction);
	// wrong signer
	EXPECT_THROW(c.run(), validate::TransactionValidationForbiddenSignException);
}

TEST(ValidateGradidoTransaction, validGradidoTransferTransaction) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(transferTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[4])
		.build()
		;
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidGradidoTransferTransactionMissingSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(transferTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.build()
		;
	validate::Context c(*transaction);
	// missing signer
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

TEST(ValidateGradidoTransaction, invalidGradidoTransferTransactionWrongSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(transferTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[3])
		.build()
		;
	validate::Context c(*transaction);
	// wrong signer, missing required signer, the sender of transaction
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}

TEST(ValidateGradidoTransaction, validGradidoDeferredTransferTransaction) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(deferredTransferTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[4])
		.build()
		;
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidGradidoDeferredTransferTransactionMissingSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(deferredTransferTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.build()
		;
	validate::Context c(*transaction);
	// missing signer
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

TEST(ValidateGradidoTransaction, invalidGradidoDeferredTransferTransactionWrongSignature) {
	GradidoTransactionBuilder builder;
	auto bodyBytes = make_shared<Block>(Block::fromBase64(deferredTransferTransactionBase64));

	auto transaction = builder
		.setTransactionBody(bodyBytes)
		.sign(g_KeyPairs[5])
		.build()
		;
	validate::Context c(*transaction);
	// wrong signer, missing required signer, the sender of transaction
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}
