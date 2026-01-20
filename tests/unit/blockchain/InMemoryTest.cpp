#include "../main.h"
#include "InMemoryTest.h"
#include "../serializedTransactions.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/serialization/toJsonString.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"

#include "date/date.h"
#include "date/tz.h"
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_iostream.hpp"
#include <chrono>

using namespace std;
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;
using namespace date;
using namespace magic_enum;
using magic_enum::iostream_operators::operator<<;

using gradido::AppContext, gradido::g_appContext;

#define VERSION_STRING "3.5"
static EncryptedMemo memo("dummy memo");
const hiero::AccountId hieroAccount(0, 0, 121);

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
	mCommunityId = "test-community";
	mLastCreatedAt = std::chrono::system_clock::from_time_t(1641681324);
	mBlockchain = InMemoryProvider::getInstance()->findBlockchain(mCommunityId);

	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(mLastCreatedAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.setSenderCommunity(mCommunityId)
		.sign(g_KeyPairs[0])
		;
	mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), mLastCreatedAt);
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

	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(generateNewCreatedAt())
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			g_KeyPairs[userPubkeyIndex]->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			g_KeyPairs[accountPubkeyIndex]->getPublicKey()
		)
		.setSenderCommunity(mCommunityId)
		.sign(g_KeyPairs[accountPubkeyIndex])
		// sign with community root key
		.sign(g_KeyPairs[0])
	;
	auto confirmedAt = generateNewConfirmedAt(mLastCreatedAt);
	ASSERT_TRUE(mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), confirmedAt));
}

std::shared_ptr<KeyPairEd25519> InMemoryTest::createRegisterAddressGenerateKeyPair()
{
	const int seedSize = 32;
	memory::Block buffer(seedSize);
	randombytes_buf(buffer, seedSize);

	auto userKeyPair = KeyPairEd25519::create(buffer);
	randombytes_buf(buffer, seedSize);
	auto accountKeyPair = KeyPairEd25519::create(buffer);
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(generateNewCreatedAt())
		.setVersionNumber(VERSION_STRING)
		.setRegisterAddress(
			userKeyPair->getPublicKey(),
			AddressType::COMMUNITY_HUMAN,
			nullptr,
			accountKeyPair->getPublicKey()
		)
		.setSenderCommunity(mCommunityId)
		// sign with community root key
		.sign(g_KeyPairs[0])
		.sign(accountKeyPair)
		.sign(userKeyPair)		
		;
	auto confirmedAt = generateNewConfirmedAt(mLastCreatedAt);
	EXPECT_TRUE(mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), confirmedAt));
	return accountKeyPair;
}

bool InMemoryTest::createGradidoCreation(
	int recipientKeyPairIndex,
	int signerKeyPairIndex,
	GradidoUnit amount,
	Timepoint createdAt,
	Timepoint targetDate
) {
	assert(recipientKeyPairIndex > 0 && recipientKeyPairIndex < g_KeyPairs.size());
	return createGradidoCreation(g_KeyPairs[recipientKeyPairIndex]->getPublicKey(), signerKeyPairIndex, amount, createdAt, targetDate);
}

bool InMemoryTest::createGradidoCreation(
	memory::ConstBlockPtr recipientPublicKey,
	int signerKeyPairIndex,
	GradidoUnit amount,
	Timepoint createdAt,
	Timepoint targetDate
) {
	assert(signerKeyPairIndex > 0 && signerKeyPairIndex < g_KeyPairs.size());
	GradidoTransactionBuilder builder;
	builder
		.addMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(recipientPublicKey, amount, 0),
			targetDate
		)
		.setRecipientCommunity(mCommunityId)
		.sign(g_KeyPairs[signerKeyPairIndex])
		;
	auto confirmedAt = generateNewConfirmedAt(createdAt);
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), confirmedAt);
}

bool InMemoryTest::createGradidoTransfer(
	int senderKeyPairIndex,
	int recipientKeyPairIndex,
	GradidoUnit amount,
	Timepoint createdAt
) {
	assert(senderKeyPairIndex > 0 && senderKeyPairIndex < g_KeyPairs.size());
	assert(recipientKeyPairIndex > 0 && recipientKeyPairIndex < g_KeyPairs.size());

	GradidoTransactionBuilder builder;
	builder
		.addMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionTransfer(
			TransferAmount(g_KeyPairs[senderKeyPairIndex]->getPublicKey(), amount, 0),
			g_KeyPairs[recipientKeyPairIndex]->getPublicKey()
		)
		.setSenderCommunity(mCommunityId)
		.sign(g_KeyPairs[senderKeyPairIndex])
	;
	auto confirmedAt = generateNewConfirmedAt(createdAt);
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), confirmedAt);
}

bool InMemoryTest::createGradidoDeferredTransfer(
	int senderKeyPairIndex,
	int recipientKeyPairIndex,
	GradidoUnit amount, 
	Timepoint createdAt,
	std::chrono::seconds timeoutDuration
) {
	assert(senderKeyPairIndex > 0 && senderKeyPairIndex < g_KeyPairs.size());
	assert(recipientKeyPairIndex > 0 && recipientKeyPairIndex < g_KeyPairs.size());

	GradidoTransactionBuilder builder;
	builder
		.addMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[senderKeyPairIndex]->getPublicKey(), amount, 0),
				g_KeyPairs[recipientKeyPairIndex]->getPublicKey()
			), DurationSeconds(timeoutDuration)
		)
		.setSenderCommunity(mCommunityId)
		.sign(g_KeyPairs[senderKeyPairIndex])
	;	
	auto confirmedAt = generateNewConfirmedAt(createdAt);
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), confirmedAt);
}


bool InMemoryTest::createGradidoRedeemDeferredTransfer(
	int senderKeyPairIndex,
	int recipientKeyPairIndex,
	GradidoUnit amount,
	Timepoint createdAt,
	uint64_t deferredTransferNr
) {
	assert(senderKeyPairIndex > 0 && senderKeyPairIndex < g_KeyPairs.size());
	assert(recipientKeyPairIndex > 0 && recipientKeyPairIndex < g_KeyPairs.size());

	GradidoTransactionBuilder builder;
	builder
		.addMemo("redeem deferred")
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRedeemDeferredTransfer(
			deferredTransferNr,
			GradidoTransfer(
				TransferAmount(g_KeyPairs[senderKeyPairIndex]->getPublicKey(), amount, 0),
				g_KeyPairs[recipientKeyPairIndex]->getPublicKey()
			)
		)
		.setSenderCommunity(mCommunityId)
		.sign(g_KeyPairs[senderKeyPairIndex])
		;
	auto confirmedAt = generateNewConfirmedAt(createdAt);
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), confirmedAt);
}

void InMemoryTest::logBlockchain()
{
	auto transactions = dynamic_cast<InMemory*>(mBlockchain.get())->getSortedTransactions();
	for (auto transaction : transactions) {
		LOG_F(INFO, "%s", serialization::toJsonString(*transaction->getConfirmedTransaction(), true).data());
	}
}

GradidoUnit InMemoryTest::getBalance(int keyPairIndex, Timepoint date)
{
	if(keyPairIndex < 0 || keyPairIndex >= g_KeyPairs.size()) {
		throw std::runtime_error("invalid key pair index");
	}
	interaction::calculateAccountBalance::Context c(mBlockchain);
	return c.fromEnd(g_KeyPairs[keyPairIndex]->getPublicKey(), date, 0);
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
	f.involvedPublicKey = g_KeyPairs[0]->getPublicKey();
	auto transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());

	// after adding two create addresses
	createRegisterAddress();
	createRegisterAddress();
	f.transactionType = TransactionType::COMMUNITY_ROOT;
	transaction = mBlockchain->findOne(f);
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
	transaction = mBlockchain->findOne(f);
	ASSERT_TRUE(transaction);
	EXPECT_TRUE(transaction->getTransactionBody()->isCommunityRoot());
}

TEST_F(InMemoryTest, RegisterSomeAddresses)
{
	ASSERT_NO_THROW(createRegisterAddress());
	ASSERT_NO_THROW(createRegisterAddress());
	ASSERT_NO_THROW(createRegisterAddress());
	Filter f;
	f.involvedPublicKey = g_KeyPairs[8]->getPublicKey();
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
		LOG_F(ERROR, "%s", ex.getFullString().data());
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
		LOG_F(ERROR, "%s", ex.getFullString().data());
	}

	// 1000.0000 decayed for 23 hours => 998.1829
	// new algo
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit(1998.1829));

	ASSERT_NO_THROW(createRegisterAddress(7));
	createdAt = generateNewCreatedAt();
	targetDate = getPreviousNMonth2(createdAt, 2);
	ASSERT_TRUE(createGradidoCreation(8, 4, 1000.0, createdAt, targetDate));
	
	EXPECT_EQ(getBalance(8, mLastConfirmedAt), GradidoUnit(1000.0));
	auto balanceCalculator = calculateAccountBalance::Context(mBlockchain);
	auto gmwBalance = balanceCalculator.fromEnd(g_KeyPairs[1]->getPublicKey(), mLastConfirmedAt);
	auto aufBalance = balanceCalculator.fromEnd(g_KeyPairs[2]->getPublicKey(), mLastConfirmedAt);
	auto creationSum = getBalance(8, mLastConfirmedAt) + getBalance(6, mLastConfirmedAt);
	EXPECT_EQ(gmwBalance, creationSum);
	EXPECT_EQ(aufBalance, creationSum);
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
		LOG_F(ERROR, "%s", ex.getFullString().data());
	}
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit(1000.0));
	auto creationConfirmedAt = mLastConfirmedAt;
	
	try {
		ASSERT_TRUE(createGradidoTransfer(6, 4, 500.10, generateNewCreatedAt()));
	} catch(GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, "%s", ex.getFullString().data());
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

TEST_F(InMemoryTest, ValidGradidoDeferredTransfer)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));
	ASSERT_NO_THROW(createRegisterAddress(7));

	auto createdAt = generateNewCreatedAt();
	auto targetDate = getPreviousNMonth2(createdAt, 1);
	// account 6 balance: 1000.0 at confirmed at date (confirmedAt = createdAt + 1 minute)
	ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));
	EXPECT_EQ(getBalance(6, mLastConfirmedAt), GradidoUnit(1000.0));

	// deferred transfer from account 6 containing only creation transaction with 1000 - decay
	createdAt = mLastCreatedAt + chrono::hours(10);
	auto firstDeferredTransferCreatedAt = createdAt;
	auto timeoutDuration = chrono::hours(24 * 60);
	const auto recipientKeyPairIndex = 9;
	auto deferredTransferAmount = GradidoUnit(500.10).calculateCompoundInterest(timeoutDuration);
	try {
		ASSERT_TRUE(createGradidoDeferredTransfer(6, recipientKeyPairIndex, deferredTransferAmount, createdAt, timeoutDuration));
	} catch(GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, "%s", ex.getFullString().data());
	}

	// check account
	auto blockedDeferredTransferBalance = GradidoUnit(500.10).calculateCompoundInterest(createdAt, createdAt + timeoutDuration);
	auto deferredTransferBalance = getBalance(recipientKeyPairIndex, mLastConfirmedAt);
	auto userBalanceAtDeferredTransferTime = getBalance(6, createdAt).calculateDecay(createdAt, mLastConfirmedAt);
	auto userBalance = getBalance(6, mLastConfirmedAt);
	auto lastUserBalanceDate = mLastConfirmedAt;
	EXPECT_EQ(userBalance, GradidoUnit(438.7963));
	auto diff = userBalance - (userBalanceAtDeferredTransferTime - blockedDeferredTransferBalance);
	// the difference should be small, normaly it should be identical but we must account for rounding errors
	EXPECT_LE(abs(diff.getGradidoCent()), 1);
	EXPECT_LT(userBalance + deferredTransferBalance, GradidoUnit(1000.0));

	// deferred transfer from deferred transfer account recipientKeyPairIndex to a new account
	createdAt = mLastConfirmedAt + chrono::hours(36);
	mLastCreatedAt = createdAt;
	auto secondTimeoutDuration = chrono::hours(24 * 30);
	const auto secondRecipientKeyPairIndex = 6;
	auto balanceWhenSecondsDeferredTransferStart = getBalance(recipientKeyPairIndex, firstDeferredTransferCreatedAt + chrono::minutes{ 1 });
	ASSERT_EQ(balanceWhenSecondsDeferredTransferStart, GradidoUnit(560.4132));
	EXPECT_THROW(
		createGradidoDeferredTransfer(recipientKeyPairIndex, secondRecipientKeyPairIndex, 483.0, createdAt, secondTimeoutDuration),
		validate::WrongAddressTypeException
	);
	try {
		// redeem from creator, only two account balances in confirmed transaction
		EXPECT_NO_THROW(createGradidoRedeemDeferredTransfer(recipientKeyPairIndex, secondRecipientKeyPairIndex, 483.0, createdAt, 6));
	}
	catch (GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, "%s", ex.getFullString().data());
	}

	auto lastTransactionEntry = mBlockchain->findOne(Filter::LAST_TRANSACTION);
	auto confirmedTransaction = lastTransactionEntry->getConfirmedTransaction();
	ASSERT_EQ(confirmedTransaction->getAccountBalances().size(), 2);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[secondRecipientKeyPairIndex]->getPublicKey(), 0).getBalance(), GradidoUnit(996.3677));
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[recipientKeyPairIndex]->getPublicKey(), 0).getBalance(), GradidoUnit::zero());
	// check accounts
	blockedDeferredTransferBalance = GradidoUnit(483.0).calculateCompoundInterest(createdAt, createdAt + secondTimeoutDuration);
	deferredTransferBalance = getBalance(recipientKeyPairIndex, firstDeferredTransferCreatedAt + timeoutDuration + chrono::hours(1));
	auto newDeferredTransferBalance = getBalance(secondRecipientKeyPairIndex, mLastConfirmedAt);
	auto userBalanceWithChange = getBalance(6, firstDeferredTransferCreatedAt + timeoutDuration + chrono::hours(1));
	auto decayedUserBalance = userBalance.calculateDecay(lastUserBalanceDate, firstDeferredTransferCreatedAt + timeoutDuration + chrono::hours(1));
	auto timeBetween = GradidoUnit::calculateDecayDurationSeconds(lastUserBalanceDate, firstDeferredTransferCreatedAt + timeoutDuration + chrono::hours(1));
	/*printf("time between: %s\n", DataTypeConverter::timespanToString(chrono::duration_cast<chrono::seconds>(timeBetween)).data());
	printf("userBalance: %s\n", userBalance.toString().data());
	auto decayFactor = pow(2.0, (-chrono::duration_cast<chrono::seconds>(timeBetween).count()/31556952.0));
	auto decay = userBalance * GradidoUnit(decayFactor);
	printf("decay factor: %f, decay: %s\n", decayFactor, decay.toString().data());
	//EXPECT_EQ(getBalance(6, mLastConfirmedAt), decayedUserBalance);
	printf("user balance with change: %s, decayed user balance: %s\n", userBalanceWithChange.toString().data(), decayedUserBalance.toString().data());
	printf("deferred transfer balance: %s\n", deferredTransferBalance.toString().data());
	printf("new deferred transfer balance: %s\n", newDeferredTransferBalance.toString().data());
	*/
	// try transfering gdd from deferred transfer 
	createdAt = generateNewCreatedAt();
	auto thirdTimeoutDuration = chrono::hours(30 * 24);
	auto thirdRecipientKeyPairIndex = 10;
	auto originalSenderBalance = getBalance(6, createdAt + std::chrono::minutes(1));
	auto deferredFullBalance = GradidoUnit(400.0).calculateCompoundInterest(thirdTimeoutDuration);
	EXPECT_NO_THROW(createGradidoDeferredTransfer(6, thirdRecipientKeyPairIndex, deferredFullBalance, createdAt, thirdTimeoutDuration));
	originalSenderBalance -= deferredFullBalance;
	lastTransactionEntry = mBlockchain->findOne(Filter::LAST_TRANSACTION);
	confirmedTransaction = lastTransactionEntry->getConfirmedTransaction();
	ASSERT_EQ(confirmedTransaction->getAccountBalances().size(), 2);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[6]->getPublicKey(), 0).getBalance(), originalSenderBalance);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[thirdRecipientKeyPairIndex]->getPublicKey(), 0).getBalance(), deferredFullBalance);

	// redeem second deferred transfer
	auto previousCreatedAt = createdAt;
	createdAt = generateNewCreatedAt();
	originalSenderBalance = originalSenderBalance.calculateDecay(previousCreatedAt, createdAt);
	deferredFullBalance = deferredFullBalance.calculateDecay(previousCreatedAt, createdAt);
	EXPECT_NO_THROW(createGradidoRedeemDeferredTransfer(thirdRecipientKeyPairIndex, 8, GradidoUnit(400.0), createdAt, 8));
	lastTransactionEntry = mBlockchain->findOne(Filter::LAST_TRANSACTION);
	confirmedTransaction = lastTransactionEntry->getConfirmedTransaction();
	ASSERT_EQ(confirmedTransaction->getAccountBalances().size(), 3);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[secondRecipientKeyPairIndex]->getPublicKey(), 0).getBalance(),
		originalSenderBalance + deferredFullBalance - GradidoUnit(400.0)
	);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[thirdRecipientKeyPairIndex]->getPublicKey(), 0).getBalance(), GradidoUnit::zero());
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[8]->getPublicKey(), 0).getBalance(), GradidoUnit(400.0));
	auto transactions = mBlockchain->findAll();
	EXPECT_EQ(transactions.size(), 9);
	// logBlockchain();
	transactions = mBlockchain->findAll(Filter::ALL_TRANSACTIONS);
	EXPECT_EQ(transactions.size(), 9);
}

TEST_F(InMemoryTest, ValidGradidoTimeoutDeferredTransfer)
{
	ASSERT_NO_THROW(createRegisterAddress(3));
	ASSERT_NO_THROW(createRegisterAddress(5));
	auto createdAt = generateNewCreatedAt();
	auto targetDate = getPreviousNMonth2(createdAt, 1);
	// account 6 balance: 1000.0 at confirmed at date (confirmedAt = createdAt + 1 minute)
	ASSERT_TRUE(createGradidoCreation(6, 4, 1000.0, createdAt, targetDate));

	// deferred transfer from account 6 containing only creation transaction with 1000 - decay
	createdAt = mLastCreatedAt + chrono::hours(10);
	auto timeoutDuration = chrono::hours(24 * 60);
	const auto recipientKeyPairIndex = 7;
	auto deferredTransferAmount = GradidoUnit(847.10).calculateCompoundInterest(timeoutDuration);
	try {
		ASSERT_TRUE(createGradidoDeferredTransfer(6, recipientKeyPairIndex, deferredTransferAmount, createdAt, timeoutDuration));
	}
	catch (GradidoBlockchainException& ex) {
		logBlockchain();
		LOG_F(ERROR, "%s", ex.getFullString().data());
	}
	// trigger timeout deferred transfer
	createdAt += timeoutDuration * 2;
	targetDate = createdAt - chrono::hours(24 * 30);
	try {
		createGradidoCreation(4, 6, 1000.0, createdAt, targetDate);
	} catch (std::exception e) {
		printf("%s\n", e.what());
		int zahl = 0;
	}
	// EXPECT_NO_THROW(createGradidoCreation(4, 6, 1000.0, createdAt, targetDate));
	// logBlockchain();
}

TEST_F(InMemoryTest, ManyTransactions)
{
	Profiler timeUsed;
	const int userCount = 100;
	Timepoint previousConfirmedAt;
	// admin
	GradidoUnit decayedAmountSum;
	ASSERT_NO_THROW(createRegisterAddress(3));
	for (int i = 0; i < userCount; i++) {
		auto accountKeyPair = createRegisterAddressGenerateKeyPair();
		auto createdAt = generateNewCreatedAt();
		auto targetDate = getPreviousNMonth2(createdAt, static_cast<int>(rand() % 3));
		uint64_t randAmount = 0;
		randombytes_buf(&randAmount, sizeof(randAmount));
		auto amount = GradidoUnit::fromGradidoCent(10000 + randAmount % 9990001);
		ASSERT_TRUE(createGradidoCreation(accountKeyPair->getPublicKey(), 4, amount, createdAt, targetDate));
		if (i > 0) {
			decayedAmountSum = decayedAmountSum.calculateDecay(previousConfirmedAt, mLastConfirmedAt);
		}
		previousConfirmedAt = mLastConfirmedAt;
		decayedAmountSum += amount;
	}

	auto firstTransaction = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
	ASSERT_EQ(firstTransaction->getTransactionNr(), 1);
	ASSERT_TRUE(firstTransaction->isCommunityRoot());

	auto lastTransaction = mBlockchain->findOne(Filter::LAST_TRANSACTION);
	ASSERT_EQ(lastTransaction->getTransactionNr(), userCount * 2 + 2);
	ASSERT_TRUE(lastTransaction->isCreation());
	Filter f;
	f.involvedPublicKey = g_KeyPairs[1]->getPublicKey();
	auto changeGmwBalanceTransactions = mBlockchain->findAll(f);
	ASSERT_EQ(changeGmwBalanceTransactions.size(), userCount + 1);
	
	// printf gmw and auf account
	calculateAccountBalance::Context balanceCalculator(mBlockchain);
	auto gmwBalance = balanceCalculator.fromEnd(g_KeyPairs[1]->getPublicKey(), mLastConfirmedAt, 0);
	auto aufBalance = balanceCalculator.fromEnd(g_KeyPairs[2]->getPublicKey(), mLastConfirmedAt, 0);
	
	ASSERT_EQ(gmwBalance, decayedAmountSum);
	ASSERT_EQ(aufBalance, decayedAmountSum);
}