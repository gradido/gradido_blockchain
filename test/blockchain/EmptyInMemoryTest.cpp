#include "EmptyInMemoryTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"

using namespace std; 
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;

const auto confirmedAt = std::chrono::system_clock::from_time_t(1609464130);
const hiero::AccountId hieroAccount(0, 0, 121);

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
	auto communityRootRaw = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	interaction::deserialize::Context deserializer(communityRootRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	interaction::serialize::Context serializeTransactionId({ confirmedAt, hieroAccount });
	ASSERT_TRUE(deserializer.isGradidoTransaction());	
	EXPECT_TRUE(mBlockchain->createAndAddConfirmedTransaction(deserializer.getGradidoTransaction(), serializeTransactionId.run(), confirmedAt));
}

TEST_F(EmptyInMemoryTest, InvalidRegisterAddressAsFirst) {
	auto registerAddressRaw = make_shared<memory::Block>(memory::Block::fromBase64(registeAddressTransactionBase64));
	interaction::deserialize::Context deserializer(registerAddressRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	interaction::serialize::Context deserializeTransactionId({ confirmedAt, hieroAccount });
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());
	interaction::serialize::Context serializeTransactionId({ confirmedAt, hieroAccount });
	EXPECT_THROW(
		mBlockchain->createAndAddConfirmedTransaction(deserializer.getGradidoTransaction(), serializeTransactionId.run(), confirmedAt),
		BlockchainOrderException
	);
}
TEST_F(EmptyInMemoryTest, InvalidGradidoCreationAsFirst) {
	auto creationRaw = make_shared<memory::Block>(memory::Block::fromBase64(creationTransactionBase64));
	interaction::deserialize::Context deserializer(creationRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());
	interaction::serialize::Context serializeTransactionId({ confirmedAt, hieroAccount });
	EXPECT_THROW(
		mBlockchain->createAndAddConfirmedTransaction(deserializer.getGradidoTransaction(), serializeTransactionId.run(), confirmedAt),
		validate::WrongAddressTypeException
	);
}
TEST_F(EmptyInMemoryTest, InvalidGradidoTransferAsFirst) {
	auto transferRaw = make_shared<memory::Block>(memory::Block::fromBase64(transferTransactionBase64));
	interaction::deserialize::Context deserializer(transferRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());
	interaction::serialize::Context serializeTransactionId({ confirmedAt, hieroAccount });
	EXPECT_THROW(
		mBlockchain->createAndAddConfirmedTransaction(deserializer.getGradidoTransaction(), serializeTransactionId.run(), confirmedAt),
		BlockchainOrderException
	);
}

TEST_F(EmptyInMemoryTest, InvalidGradidoDeferredTransferAsFirst) {
	auto deferredTransferRaw = make_shared<memory::Block>(memory::Block::fromBase64(deferredTransferTransactionBase64));
	interaction::deserialize::Context deserializer(deferredTransferRaw, interaction::deserialize::Type::GRADIDO_TRANSACTION);
	deserializer.run();
	ASSERT_TRUE(deserializer.isGradidoTransaction());
	interaction::serialize::Context serializeTransactionId({ confirmedAt, hieroAccount });
	EXPECT_THROW(
		mBlockchain->createAndAddConfirmedTransaction(deserializer.getGradidoTransaction(), serializeTransactionId.run(), confirmedAt),
		BlockchainOrderException
	);
}