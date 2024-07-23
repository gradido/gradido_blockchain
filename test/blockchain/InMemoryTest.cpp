#include "InMemoryTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

using namespace std;
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;

#define VERSION_STRING "3.3"


void InMemoryTest::SetUp()
{
	random_device rd;  // a seed source for the random number engine
	gen = mt19937(rd()); // mersenne_twister_engine seeded with rd()
	auto randTimeRange = uniform_int_distribution<int>(2400, 2 * 24 * 60 * 60);
	mKeyPairCursor = 3;
	mCommunityId = "testCommunity";
	mLastCreatedAt = std::chrono::system_clock::from_time_t(1609459200);
	mBlockchain = InMemoryProvider::getInstance()->findBlockchain(mCommunityId);
	mAccountKeyPairIndices.reserve(g_KeyPairs.size());

	auto transaction = make_shared<GradidoTransaction>() ;
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);
	mBlockchain->addGradidoTransaction(transaction, nullptr, mLastCreatedAt);
	
}

void InMemoryTest::TearDown()
{
	InMemoryProvider::getInstance()->clear();
	mAccountKeyPairIndices.clear();
}

Timepoint InMemoryTest::generateNewCreatedAt()
{
	mLastCreatedAt += std::chrono::seconds{ randTimeRange(gen) }; // generate a random duration
	return mLastCreatedAt;
}

void InMemoryTest::createRegisterAddress()
{
	if (mKeyPairCursor + 1 >= g_KeyPairs.size()) {
		throw std::runtime_error("not enough key pairs");
	}
	createRegisterAddress(mKeyPairCursor);
	mKeyPairCursor += 2;
}


void InMemoryTest::createRegisterAddress(int keyPairIndexStart)
{
	if (keyPairIndexStart + 1 >= g_KeyPairs.size()) {
		throw std::runtime_error("not enough key pairs");
	}
	TransactionBody body("", generateNewCreatedAt(), VERSION_STRING);
	auto userPubkeyIndex    = keyPairIndexStart;
	auto accountPubkeyÍndex = keyPairIndexStart + 1;
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[userPubkeyIndex].publicKey,
		nullptr,
		g_KeyPairs[accountPubkeyÍndex].publicKey
	);
	mAccountKeyPairIndices.push_back(accountPubkeyÍndex);
	serialize::Context c(body);
	auto transaction = make_shared<GradidoTransaction>();
	transaction->bodyBytes = c.run();
	sign(*transaction, g_KeyPairs[accountPubkeyÍndex]);
	// sign with community root key
	sign(*transaction, g_KeyPairs[0]);
	ASSERT_TRUE(mBlockchain->addGradidoTransaction(transaction, nullptr, mLastCreatedAt));
}

TEST_F(InMemoryTest, FindCommunityRootTransactionByType)
{
	Filter f;
	f.transactionType = TransactionType::COMMUNITY_ROOT;
	auto transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());

	// after adding two create addresses
	createRegisterAddress();
	createRegisterAddress();
	transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());
}

TEST_F(InMemoryTest, FindCommunityRootTransactionByPublicKey)
{
	Filter f;
	f.involvedPublicKey = g_KeyPairs[0].publicKey;
	auto transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());

	// after adding two create addresses
	createRegisterAddress();
	createRegisterAddress();
	transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());
}

TEST_F(InMemoryTest, RegisterSomeAddresses)
{
	ASSERT_NO_THROW(createRegisterAddress());
	ASSERT_NO_THROW(createRegisterAddress());
	ASSERT_NO_THROW(createRegisterAddress());
}

TEST_F(InMemoryTest, InvalidRegisterAddressAgain)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	EXPECT_THROW(createRegisterAddress(3), validate::AddressAlreadyExistException);
}