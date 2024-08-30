#include "InMemoryTest.h"
#include "../serializedTransactions.h"

#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/toJson/Context.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"


#include "date/date.h"
#include "date/tz.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_iostream.hpp"

using namespace std;
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;
using namespace date;
using namespace magic_enum;
using magic_enum::iostream_operators::operator<<;

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
	randTimeRange = uniform_int_distribution<int>(2400, 2 * 24 * 60 * 60);
	mKeyPairCursor = 3;
	mCommunityId = "testCommunity";
	mLastCreatedAt = std::chrono::system_clock::from_time_t(1641681324);
	mBlockchain = InMemoryProvider::getInstance()->findBlockchain(mCommunityId);
		
	auto bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));

	// keyPair 0 is public of this community root Transaction
	auto signature = sign(bodyBytes, g_KeyPairs[0]);
	auto transaction = make_shared<GradidoTransaction>();
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[0].publicKey, signature)
	;
	mBlockchain->addGradidoTransaction(builder.build(), nullptr, mLastCreatedAt);
}

void InMemoryTest::TearDown()
{
	InMemoryProvider::getInstance()->clear();
}

Timepoint InMemoryTest::generateNewCreatedAt()
{
	mLastCreatedAt += std::chrono::seconds{ randTimeRange(gen) }; // generate a random duration
	return mLastCreatedAt;
}

Timepoint InMemoryTest::generateNewConfirmedAt(Timepoint createdAt)
{
	mLastConfirmedAt = createdAt + chrono::minutes{1};
	return mLastConfirmedAt;
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

	auto userPubkeyIndex = keyPairIndexStart;
	auto accountPubkeyIndex = keyPairIndexStart + 1;
	TransactionBodyBuilder bodyBuilder;
	bodyBuilder
		.setCreatedAt(generateNewCreatedAt())
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[userPubkeyIndex].publicKey,
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[accountPubkeyIndex].publicKey
		)
	;
	auto body = bodyBuilder.build();
	serialize::Context c(*body);
	auto bodyBytes = c.run();
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[accountPubkeyIndex].publicKey, sign(bodyBytes, g_KeyPairs[accountPubkeyIndex]))
		// sign with community root key
		.addSignaturePair(g_KeyPairs[0].publicKey, sign(bodyBytes, g_KeyPairs[0]))
	;
		
	ASSERT_TRUE(mBlockchain->addGradidoTransaction(builder.build(), nullptr, generateNewConfirmedAt(mLastCreatedAt)));
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
	TransactionBodyBuilder bodyBuilder;
	bodyBuilder
		.setMemo("dummy memo")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[recipientKeyPairIndex].publicKey, amount),
			targetDate
		)
		;
	auto body = bodyBuilder.build();

	serialize::Context c(*body);
	auto bodyBytes = c.run();
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[signerKeyPairIndex].publicKey, sign(bodyBytes, g_KeyPairs[signerKeyPairIndex]))
	;	
	return mBlockchain->addGradidoTransaction(builder.build(), nullptr, generateNewConfirmedAt(createdAt));
}

bool InMemoryTest::createGradidoTransfer(
	int senderKeyPairIndex,
	int recipientKeyPairIndex,
	GradidoUnit amount, 
	Timepoint createdAt
) {
	assert(senderKeyPairIndex > 0 && senderKeyPairIndex < g_KeyPairs.size());
	assert(recipientKeyPairIndex > 0 && recipientKeyPairIndex < g_KeyPairs.size());

	TransactionBodyBuilder bodyBuilder;
	bodyBuilder
		.setMemo("dummy memo")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[senderKeyPairIndex].publicKey, amount),
			g_KeyPairs[recipientKeyPairIndex].publicKey
		)
		;
	auto body = bodyBuilder.build();

	serialize::Context c(*body);
	auto bodyBytes = c.run();
	GradidoTransactionBuilder builder;
	builder
		.setTransactionBody(bodyBytes)
		.addSignaturePair(g_KeyPairs[senderKeyPairIndex].publicKey, sign(bodyBytes, g_KeyPairs[senderKeyPairIndex]))
	;	
	return mBlockchain->addGradidoTransaction(builder.build(), nullptr, generateNewConfirmedAt(createdAt));
}

void InMemoryTest::logBlockchain()
{
	auto transactions = dynamic_cast<InMemory*>(mBlockchain.get())->getSortedTransactions();

	for (auto transaction : transactions) {
		toJson::Context c(*transaction->getConfirmedTransaction());
		LOG_F(INFO, c.run(true).data());
	}
}

GradidoUnit InMemoryTest::getBalance(int keyPairIndex, Timepoint date)
{
	if(keyPairIndex < 0 || keyPairIndex >= g_KeyPairs.size()) {
		throw std::runtime_error("invalid key pair index");
	}
	interaction::calculateAccountBalance::Context c(*mBlockchain);
	return c.run(g_KeyPairs[keyPairIndex].publicKey, date);
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
	try {
		ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	} catch(GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, ex.getFullString().data());
	}
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit(1000.0));
	
	createdAt += chrono::hours{ 23 };
	mLastCreatedAt = createdAt;
	auto newTargetDate = getPreviousNMonth2(createdAt, 2);
	if (date::year_month_day(floor<days>(newTargetDate)).month() == date::year_month_day(floor<days>(targetDate)).month()) {
		newTargetDate = getPreviousNMonth2(createdAt, 1);
	}
	try {
		ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, newTargetDate));
	}
	catch (GradidoBlockchainException& ex) {
		loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
		logBlockchain();
		auto createAtString = DataTypeConverter::timePointToString(createdAt);
		auto targetDateString = DataTypeConverter::timePointToString(newTargetDate);
		LOG_F(INFO, "createdAt: %s, targetDate: %s", createAtString.data(), targetDateString.data());
		LOG_F(ERROR, ex.getFullString().data());
	}

	// 1000.0000 decayed for 23 hours => 998.1829
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit(1998.1829));

	ASSERT_NO_THROW(createRegisterAddress(7));
	createdAt = generateNewCreatedAt();
	targetDate = getPreviousNMonth2(createdAt, 2);
	try {
		ASSERT_TRUE(createGradidoCreation(8, 4, 1000.0, createdAt, targetDate));
	} catch(GradidoBlockchainException& ex) {
		logBlockchain();
		blockchain::Filter filter;
		filter.involvedPublicKey = make_shared<memory::Block>(
		  memory::Block::fromHex("8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57")
		);
		filter.searchDirection = blockchain::SearchDirection::DESC;
		//filter.timepointInterval = TimepointInterval(mBlockchain->getStartDate(), createdAt);
		std::cout << mBlockchain->getStartDate() << " - " << createdAt << std::endl;
		auto results = mBlockchain->findAll(filter);
		std::cout << results.size() << std::endl;
		LOG_F(ERROR, ex.getFullString().data());
	}
	EXPECT_EQ(getBalance(8, mLastConfirmedAt), GradidoUnit(1000.0));
}


TEST_F(InMemoryTest, InvalidCreationTransactions)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));
	auto createdAt = generateNewCreatedAt();
	auto targetDate = getPreviousNMonth2(createdAt, 1);
	auto succedCreatedAd = createdAt;
	// valid creation
	ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	auto confirmedAtValidCreation = mLastConfirmedAt;
	createdAt += chrono::seconds{ 120 };
	ASSERT_THROW(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate), validate::InvalidCreationException);
	createdAt += chrono::hours{ 10 };
	targetDate = getPreviousNMonth2(createdAt, 3);
	//std::cout << "createdAt: " << createdAt << ", targetDate: " << targetDate << std::endl;
	ASSERT_THROW(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate), validate::TransactionValidationInvalidInputException);
	// balance from first creation, but 10 hours and 3 minutes later
	auto decayed = GradidoUnit(1000.0).calculateDecay(confirmedAtValidCreation, mLastConfirmedAt);
	EXPECT_EQ(decayed, GradidoUnit(999.2069));
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), decayed);

	ASSERT_NO_THROW(createRegisterAddress(7));
	createdAt = generateNewCreatedAt();
	targetDate = getPreviousNMonth2(createdAt, 3);
	ASSERT_THROW(createGradidoCreation(8, 4, 1000.0, createdAt, targetDate), gradido::interaction::validate::TransactionValidationInvalidInputException);
	EXPECT_EQ(getBalance(8, mLastConfirmedAt), GradidoUnit(0.0));
}

TEST_F(InMemoryTest, ValidTransferTransaction)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));
	auto createdAt = generateNewCreatedAt();
	auto targetDate = getPreviousNMonth2(createdAt, 1);
	try {
		ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	} catch(GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, ex.getFullString().data());
	}
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit(1000.0));
	auto creationConfirmedAt = mLastConfirmedAt;
	
	try {
		ASSERT_TRUE(createGradidoTransfer(6, 4, 500.10, generateNewCreatedAt()));
	} catch(GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, ex.getFullString().data());
	}

	EXPECT_EQ(getBalance(4, mLastConfirmedAt), GradidoUnit(500.1));
	auto decayed = GradidoUnit(1000.0).calculateDecay(creationConfirmedAt, mLastConfirmedAt) - GradidoUnit(500.1);
	EXPECT_LE(decayed, GradidoUnit(499.90));
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), decayed);
}

TEST_F(InMemoryTest, InvalidTransferTransaction)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));

	ASSERT_THROW(createGradidoTransfer(6, 4, 500.10, generateNewCreatedAt()), InsufficientBalanceException);

	EXPECT_EQ(getBalance(4, mLastConfirmedAt), GradidoUnit::zero());
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit::zero());
}

