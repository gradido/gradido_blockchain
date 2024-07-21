#include "InMemoryTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

using namespace std;
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;

#define VERSION_STRING "3.3"
constexpr auto communityId = "testCommunity";


void InMemoryTest::SetUp()
{
	random_device rd;  // a seed source for the random number engine
	gen = mt19937(rd()); // mersenne_twister_engine seeded with rd()
	auto randTimeRange = uniform_int_distribution<int>(2400, 2 * 24 * 60 * 60);
	mKeyPairCursor = 3;
	mLastCreatedAt = std::chrono::system_clock::from_time_t(1609459200);
	mAccountKeyPairIndices.reserve(g_KeyPairs.size());


	auto transaction = make_shared<GradidoTransaction>() ;
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);
	InMemoryProvider::getInstance()->findBlockchain(communityId)->addGradidoTransaction(transaction, nullptr, mLastCreatedAt);
	
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
	if (mKeyPairCursor + 2 >= g_KeyPairs.size()) {
		throw std::runtime_error("not enough key pairs");
	}
	TransactionBody body("", generateNewCreatedAt(), VERSION_STRING);
	auto userPubkeyIndex = mKeyPairCursor++;
	auto accountPubkeyÍndex = mKeyPairCursor++;
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
	ASSERT_TRUE(InMemoryProvider::getInstance()->findBlockchain(communityId)->addGradidoTransaction(transaction, nullptr, mLastCreatedAt));
}

TEST_F(InMemoryTest, RegisterSomeAddresses)
{
	createRegisterAddress();
	createRegisterAddress();
	createRegisterAddress();
}