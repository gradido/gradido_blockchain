#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gtest/gtest.h"

using namespace hiero;
TEST(HieroTransactionIdTest, Constrcutor)
{
	auto transactionId = TransactionId("0.0.256009@1755503343.736000193");
	EXPECT_EQ(transactionId.getAccountId().toString(), "0.0.256009");
	EXPECT_EQ(transactionId.getAccountId().getAccountNum(), 256009);
	EXPECT_EQ(transactionId.getTransactionValidStart().getSeconds(), 1755503343);
	EXPECT_EQ(transactionId.getTransactionValidStart().getNanos(), 736000193);
	

	transactionId = TransactionId("0.0.2-17281772-21022");
	EXPECT_EQ(transactionId.getAccountId().toString(), "0.0.2");
	EXPECT_EQ(transactionId.getAccountId().getAccountNum(), 2);
	EXPECT_EQ(transactionId.getTransactionValidStart().getSeconds(), 17281772);
	EXPECT_EQ(transactionId.getTransactionValidStart().getNanos(), 21022);
}

TEST(HieroTransactionIdTest, toString)
{
	auto transactionId = TransactionId(gradido::data::Timestamp(171627121, 2912), AccountId(0, 0, 1233));
	EXPECT_EQ(transactionId.toString(), "0.0.1233@171627121.2912");
}