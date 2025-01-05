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

#define VERSION_STRING "3.4"
static EncryptedMemo memo(MemoKeyType::PLAIN, std::make_shared<memory::Block>("dummy memo"));

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
		
	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(mLastCreatedAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	mBlockchain->createAndAddConfirmedTransaction(builder.build(), nullptr, mLastCreatedAt);
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
		.sign(g_KeyPairs[accountPubkeyIndex])
		// sign with community root key
		.sign(g_KeyPairs[0])
	;
		
	ASSERT_TRUE(mBlockchain->createAndAddConfirmedTransaction(builder.build(), nullptr, generateNewConfirmedAt(mLastCreatedAt)));
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
	
	GradidoTransactionBuilder builder;
	builder
		.addMemo(memo)
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setTransactionCreation(
			TransferAmount(g_KeyPairs[recipientKeyPairIndex]->getPublicKey(), amount),
			targetDate
		)
		.sign(g_KeyPairs[signerKeyPairIndex])
	;	
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), nullptr, generateNewConfirmedAt(createdAt));
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
			TransferAmount(g_KeyPairs[senderKeyPairIndex]->getPublicKey(), amount),
			g_KeyPairs[recipientKeyPairIndex]->getPublicKey()
		)
		.sign(g_KeyPairs[senderKeyPairIndex])
	;	
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), nullptr, generateNewConfirmedAt(createdAt));
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
		.addMemo({ MemoKeyType::PLAIN, std::make_shared<memory::Block>("dummy memo") })
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setDeferredTransfer(
			GradidoTransfer(
				TransferAmount(g_KeyPairs[senderKeyPairIndex]->getPublicKey(), amount),
				g_KeyPairs[recipientKeyPairIndex]->getPublicKey()
			), timeoutDuration
		)
		.sign(g_KeyPairs[senderKeyPairIndex])
	;	
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), nullptr, generateNewConfirmedAt(createdAt));
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
		.addMemo({ MemoKeyType::PLAIN, std::make_shared<memory::Block>("redeem deferred") })
		.setCreatedAt(createdAt)
		.setVersionNumber(VERSION_STRING)
		.setRedeemDeferredTransfer(
			deferredTransferNr,
			GradidoTransfer(
				TransferAmount(g_KeyPairs[senderKeyPairIndex]->getPublicKey(), amount),
				g_KeyPairs[recipientKeyPairIndex]->getPublicKey()
			)
		)
		.sign(g_KeyPairs[senderKeyPairIndex])
		;
	return mBlockchain->createAndAddConfirmedTransaction(builder.build(), nullptr, generateNewConfirmedAt(createdAt));
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
	interaction::calculateAccountBalance::Context c(mBlockchain);
	return c.fromEnd(g_KeyPairs[keyPairIndex]->getPublicKey(), date);
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
		  memory::Block::fromHex("8a8c93293cb97e8784178da8ae588144f7c982f4658bfd35101a1e2b479c3e57", 64)
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
		LOG_F(ERROR, ex.getFullString().data());
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
		LOG_F(ERROR, ex.getFullString().data());
	}

	auto lastTransactionEntry = mBlockchain->findOne(Filter::LAST_TRANSACTION);
	auto confirmedTransaction = lastTransactionEntry->getConfirmedTransaction();
	ASSERT_EQ(confirmedTransaction->getAccountBalances().size(), 2);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[secondRecipientKeyPairIndex]->getPublicKey()).getBalance(), GradidoUnit(996.3677));
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[recipientKeyPairIndex]->getPublicKey()).getBalance(), GradidoUnit::zero());
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
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[6]->getPublicKey()).getBalance(), originalSenderBalance);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[thirdRecipientKeyPairIndex]->getPublicKey()).getBalance(), deferredFullBalance);

	// redeem second deferred transfer
	auto previousCreatedAt = createdAt;
	createdAt = generateNewCreatedAt();
	originalSenderBalance = originalSenderBalance.calculateDecay(previousCreatedAt, createdAt);
	deferredFullBalance = deferredFullBalance.calculateDecay(previousCreatedAt, createdAt);
	EXPECT_NO_THROW(createGradidoRedeemDeferredTransfer(thirdRecipientKeyPairIndex, 8, GradidoUnit(400.0), createdAt, 8));
	lastTransactionEntry = mBlockchain->findOne(Filter::LAST_TRANSACTION);
	confirmedTransaction = lastTransactionEntry->getConfirmedTransaction();
	ASSERT_EQ(confirmedTransaction->getAccountBalances().size(), 3);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[secondRecipientKeyPairIndex]->getPublicKey()).getBalance(),
		originalSenderBalance + deferredFullBalance - GradidoUnit(400.0)
	);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[thirdRecipientKeyPairIndex]->getPublicKey()).getBalance(), GradidoUnit::zero());
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[8]->getPublicKey()).getBalance(), GradidoUnit(400.0));	
	// logBlockchain();
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
		LOG_F(ERROR, ex.getFullString().data());
	}
	// trigger timeout deferred transfer
	createdAt += timeoutDuration * 2;
	targetDate = createdAt - chrono::hours(24 * 30);
	EXPECT_NO_THROW(createGradidoCreation(4, 6, 1000.0, createdAt, targetDate));
	// logBlockchain();

}