#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;


TEST(ValidateGradidoTransferTest, Valid) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*transaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransferTest, Outbound) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity("019e347c-540a-724d-991c-9af318e952fe")
		.setRecipientCommunity("019e347c-540a-7e9e-8966-a70f1dc10d72")
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.buildOutbound();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*body);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateGradidoTransferTest, invalidMemoEmpty) {
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*body);
	// empty memo
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, invalidMemoToShort) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(hallMemoString)
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*body);
	// to short
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, invalidMemoToBig) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(aFilledMemoString) // fill with 451 x a
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*body);

	// to big
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}



TEST(ValidateGradidoTransferTest, InvalidAmountZero) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::zero(), communityIdIndex), // zero amount
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateGradidoTransferTest, InvalidAmountNegative) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(1000000).negated(), communityIdIndex), // negative amount
			g_KeyPairs[5]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());
	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}


TEST(ValidateGradidoTransferTest, SenderAndRecipientIdentical) {
	GradidoTransactionBuilder builder;
	builder
		.addMemo(transferMemoString)
		.setCreatedAt(createdAt)		
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			g_KeyPairs[4]->getPublicKey()
		)
		.setSenderCommunity(communityId)
		.sign(g_KeyPairs[4])
		;
	auto transaction = builder.build();
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body->isTransfer());

	validate::Context c(*body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateGradidoTransferTest, NullptrRecipientPublicKey) {
	EXPECT_THROW(
		GradidoTransfer(TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex), nullptr),
		GradidoNullPointerException
	);
}


TEST(ValidateGradidoTransferTest, EmptyRecipientPublicKey) {
	EXPECT_THROW(
		GradidoTransfer(TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex), std::make_shared<memory::Block>(32)),
		GradidoNodeInvalidDataException
	);
}

TEST(ValidateGradidoTransferTest, InvalidRecipientPublicKey) {
	EXPECT_THROW(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4]->getPublicKey(), GradidoUnit::fromGradidoCent(5005500), communityIdIndex),
			std::make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18))
		), Ed25519InvalidKeyException
	);
}
