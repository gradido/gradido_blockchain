#include "EmptyInMemoryTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"

using namespace std; 
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;

const auto confirmedAt = std::chrono::system_clock::from_time_t(1609464130);

void EmptyInMemoryTest::SetUp()
{
	mBlockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
}

void EmptyInMemoryTest::TearDown()
{
	InMemoryProvider::getInstance()->clear();
}


TEST_F(EmptyInMemoryTest, AddCommunityRootAsFirst) {
	
	// keyPair 0 is public of this community root Transaction
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
	;
	
	EXPECT_TRUE(mBlockchain->addGradidoTransaction(builder.build(), nullptr, confirmedAt));
}

TEST_F(EmptyInMemoryTest, InvalidMissingSignature) {
	// keyPair 0 is public of this community root Transaction
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
	;
	EXPECT_THROW(
		mBlockchain->addGradidoTransaction(builder.build(), nullptr, confirmedAt),
		validate::TransactionValidationMissingSignException
	);
}

TEST_F(EmptyInMemoryTest, InvalidRegisterAddressAsFirst) {
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(registeAddressTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
		.addSignaturePair(g_KeyPairs[4].publicKey, sign(bodyBytes, g_KeyPairs[4]))
	;
	EXPECT_THROW(
		mBlockchain->addGradidoTransaction(builder.build(), nullptr, confirmedAt),
		BlockchainOrderException
	);
}
TEST_F(EmptyInMemoryTest, InvalidGradidoCreationAsFirst) {
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(creationTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[3].publicKey, sign(bodyBytes, g_KeyPairs[3]))
	;
	EXPECT_THROW(
		mBlockchain->addGradidoTransaction(builder.build(), nullptr, confirmedAt),
		validate::WrongAddressTypeException
	);
}
TEST_F(EmptyInMemoryTest, InvalidGradidoTransferAsFirst) {
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(transferTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[4].publicKey, sign(bodyBytes, g_KeyPairs[4]))
	;
	EXPECT_THROW(
		mBlockchain->addGradidoTransaction(builder.build(), nullptr, confirmedAt),
		InsufficientBalanceException
	);
}

TEST_F(EmptyInMemoryTest, InvalidGradidoDeferredTransferAsFirst) {
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(deferredTransferTransactionBase64));
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[4].publicKey, sign(bodyBytes, g_KeyPairs[4]))
		;
	EXPECT_THROW(
		mBlockchain->addGradidoTransaction(builder.build(), nullptr, confirmedAt),
		InsufficientBalanceException
	);
}