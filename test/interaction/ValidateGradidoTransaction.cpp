#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "const.h"
#include "../serializedTransactions.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;
using namespace memory;

TEST(ValidateGradidoTransaction, invalidBody) 
{
	constexpr auto invalidBodyString =
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson ";

	GradidoTransactionBuilder builder;	
	EXPECT_THROW(builder.setTransactionBody(make_shared<Block>(invalidBodyString)), GradidoTransactionBuilderException);
}

TEST(ValidateGradidoTransaction, validCommunityRootGradidoTransaction) 
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	// no signature at all
	EXPECT_NO_THROW(c.run());
}


TEST(ValidateGradidoTransaction, invalidCommunityRootWrongSigner) 
{
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[1])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	// no signature at all
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}

TEST(ValidateGradidoTransaction, validRegisterAddressTransaction) 
{
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

	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidRegisterAddressTransactionMissingSignature) 
{
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
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	// still missing signature
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);
}

TEST(ValidateGradidoTransaction, invalidRegisterAddressTransactionMissingRequiredSignature) 
{

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
		.sign(g_KeyPairs[3])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	// account public key set but signed with user pubkey, so sign with account pubkey is still missing
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}


TEST(ValidateGradidoTransaction, validGradidoCreationTransaction) 
{
	GradidoTransactionBuilder builder;
	builder
		.addMemo(creationMemo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), 10000000ll),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[6])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

// creation transaction must be signed by another user
TEST(ValidateGradidoTransaction, invalidGradidoCreationTransactionWrongSignature) 
{
	GradidoTransactionBuilder builder;
	builder
		.addMemo(creationMemo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), 10000000ll),
			TimestampSeconds(1609459000)
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	// wrong signer
	EXPECT_THROW(c.run(), validate::TransactionValidationForbiddenSignException);
}

TEST(ValidateGradidoTransaction, validGradidoTransferTransaction) 
{
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), 5005500ll),
			g_KeyPairs[5]->getPublicKey()
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransaction, invalidGradidoTransferTransactionWrongSignature) 
{
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), 5005500ll),
			g_KeyPairs[5]->getPublicKey()
		)
		.sign(g_KeyPairs[3])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	// wrong signer, missing required signer, the sender of transaction
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}

TEST(ValidateGradidoTransaction, validGradidoDeferredTransferTransaction)
{
	GradidoTransactionBuilder builder;
	builder
		.addMemo(deferredTransferMemo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), 5555500ll),
				g_KeyPairs[5]->getPublicKey()
			),
			timeoutDuration
		)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();

	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}


TEST(ValidateGradidoTransaction, invalidGradidoDeferredTransferTransactionWrongSignature) 
{
	GradidoTransactionBuilder builder;
	builder
		.addMemo(deferredTransferMemo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[4]->getPublicKey(), 5555500ll),
				g_KeyPairs[5]->getPublicKey()
			),
			timeoutDuration
		)
		.sign(g_KeyPairs[5])
		;
	auto transaction = builder.build();
	validate::Context c(*transaction);
	// wrong signer, missing required signer, the sender of transaction
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}
