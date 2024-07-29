#include "InMemoryTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/Profiler.h"

#include "date/date.h"
#include "date/tz.h"

using namespace std;
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;
using namespace date;

#define VERSION_STRING "3.3"

Timepoint getPreviousNMonth2(const Timepoint& startDate, int monthsAgo) {
    auto ymd = date::year_month_day(floor<days>(startDate));
    auto month = ymd.month() - date::months(monthsAgo);
		ymd -= date::months(monthsAgo);
    int subtractDays = 1;
    while(!ymd.ok()) {
      ymd = date::year_month_day(floor<days>(startDate) - days{subtractDays++});
      ymd -= date::months(monthsAgo);
    }
		return date::sys_days(ymd);
}

void InMemoryTest::SetUp()
{
	random_device rd;  // a seed source for the random number engine
	gen = mt19937(rd()); // mersenne_twister_engine seeded with rd()
	auto randTimeRange = uniform_int_distribution<int>(2400, 2 * 24 * 60 * 60);
	mKeyPairCursor = 3;
	mCommunityId = "testCommunity";
	mLastCreatedAt = std::chrono::system_clock::from_time_t(1609459200);
	mBlockchain = InMemoryProvider::getInstance()->findBlockchain(mCommunityId);

	auto transaction = make_shared<GradidoTransaction>();
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
	mKeyPairIndexAccountMap.clear();
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
	auto userPubkeyIndex = keyPairIndexStart;
	auto accountPubkeyIndex = keyPairIndexStart + 1;
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[userPubkeyIndex].publicKey,
		nullptr,
		g_KeyPairs[accountPubkeyIndex].publicKey
	);
	mKeyPairIndexAccountMap.insert({ accountPubkeyIndex, accountPubkeyIndex });
	serialize::Context c(body);
	auto transaction = make_shared<GradidoTransaction>();
	transaction->bodyBytes = c.run();
	sign(*transaction, g_KeyPairs[accountPubkeyIndex]);
	// sign with community root key
	sign(*transaction, g_KeyPairs[0]);
	ASSERT_TRUE(mBlockchain->addGradidoTransaction(transaction, nullptr, mLastCreatedAt));
}

bool InMemoryTest::createGradidoCreation(
	int recipientKeyPairIndex,
	int signerKeyPairIndex,
	GradidoUnit amount,
	Timepoint createdAt,
	Timepoint targetDate
) {
	assert(recipientKeyPairIndex > 0 && recipientKeyPairIndex < g_KeyPairs.size());
	assert(signerKeyPairIndex > 0 && signerKeyPairIndex < g_KeyPairs.size());
	TransactionBody body("dummy memo", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[recipientKeyPairIndex].publicKey, amount),
		targetDate
	);

	serialize::Context c(body);
	auto transaction = make_shared<GradidoTransaction>();
	transaction->bodyBytes = c.run();
	sign(*transaction, g_KeyPairs[signerKeyPairIndex]);
	if (mBlockchain->addGradidoTransaction(transaction, nullptr, createdAt + chrono::seconds{ 45 })) {
		auto lastTransaction = mBlockchain->findOne(Filter::LAST_TRANSACTION)->getConfirmedTransaction();
		auto accountIt = mKeyPairIndexAccountMap.find(recipientKeyPairIndex);
		accountIt->second.balance = lastTransaction->accountBalance;
		accountIt->second.balanceDate = lastTransaction->confirmedAt;
		return true;
	}
	return false;
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
	Profiler timeUsed;
	transaction = mBlockchain->findOne(f);
	printf("time looking for transactionType: %s\n", timeUsed.string().c_str());
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
	Profiler timeUsed;
	transaction = mBlockchain->findOne(f);
	printf("time looking for publicKey: %s\n", timeUsed.string().c_str());
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());
}

TEST_F(InMemoryTest, FindCommunityRootTransactionByTransactionNr)
{
	Filter f;
	f.minTransactionNr = 1;
	f.maxTransactionNr = 1;
	auto transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());

	// after adding two create addresses
	createRegisterAddress();
	createRegisterAddress();
	Profiler timeUsed;
	transaction = mBlockchain->findOne(f);
	printf("time looking for transactionNr: %s\n", timeUsed.string().c_str());
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());
}

TEST_F(InMemoryTest, RegisterSomeAddresses)
{
	ASSERT_NO_THROW(createRegisterAddress());
	ASSERT_NO_THROW(createRegisterAddress());
	ASSERT_NO_THROW(createRegisterAddress());
	Filter f;
	f.involvedPublicKey = g_KeyPairs[8].publicKey;
	auto transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isRegisterAddress());
}

TEST_F(InMemoryTest, InvalidRegisterAddressAgain)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	EXPECT_THROW(createRegisterAddress(3), validate::AddressAlreadyExistException);
}

TEST_F(InMemoryTest, CreationTransactions)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));
	auto createdAt = generateNewCreatedAt();
	auto targetDate = getPreviousNMonth2(createdAt, 1);
	ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	auto accountIt = mKeyPairIndexAccountMap.find(6);
	EXPECT_EQ(accountIt->second.balance, GradidoUnit(1000.0));
	EXPECT_GT(accountIt->second.balanceDate, createdAt);

	createdAt += chrono::hours{ 23 };
	targetDate = getPreviousNMonth2(createdAt, 2);
	ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	// 1000.0000 decayed for 23 hours => 998.1829
	EXPECT_EQ(accountIt->second.balance, GradidoUnit(1998.1829));
	EXPECT_GT(accountIt->second.balanceDate, createdAt);

	ASSERT_NO_THROW(createRegisterAddress(7));
	createdAt = generateNewCreatedAt();
	targetDate = getPreviousNMonth2(createdAt, 2);
	ASSERT_TRUE(createGradidoCreation(8, 4, 1000.0, createdAt, targetDate));
	accountIt = mKeyPairIndexAccountMap.find(8);
	EXPECT_EQ(accountIt->second.balance, GradidoUnit(1000.0));
	EXPECT_GT(accountIt->second.balanceDate, createdAt);
}


TEST_F(InMemoryTest, InvalidCreationTransactions)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));
	auto createdAt = generateNewCreatedAt();
	auto targetDate = getPreviousNMonth2(createdAt, 1);
	auto succedCreatedAd = createdAt;
	ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	createdAt += chrono::seconds{ 120 };
	ASSERT_THROW(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate), validate::InvalidCreationException);
	createdAt += chrono::hours{ 10 };
	targetDate = getPreviousNMonth2(createdAt, 3);
	//std::cout << "createdAt: " << createdAt << ", targetDate: " << targetDate << std::endl;
	ASSERT_THROW(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate), validate::TransactionValidationInvalidInputException);
	auto accountIt = mKeyPairIndexAccountMap.find(6);
	EXPECT_EQ(accountIt->second.balance, GradidoUnit(1000.0));
	EXPECT_GT(accountIt->second.balanceDate, succedCreatedAd);

	ASSERT_NO_THROW(createRegisterAddress(7));
	createdAt = generateNewCreatedAt();
	targetDate = getPreviousNMonth2(createdAt, 3);
	ASSERT_THROW(createGradidoCreation(8, 4, 1000.0, createdAt, targetDate), validate::TransactionValidationInvalidInputException);
	accountIt = mKeyPairIndexAccountMap.find(8);
	EXPECT_EQ(accountIt->second.balance, GradidoUnit(0.0));
}

