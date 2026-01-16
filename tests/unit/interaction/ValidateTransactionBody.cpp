#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateTransactionBody, InvalidMissingSpecificTransaction)
{
	TransactionBody body(createdAt, GRADIDO_TRANSACTION_BODY_VERSION_STRING);
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

TEST(ValidateTransactionBody, InvalidVersion)
{
	TransactionBody body(createdAt, "3.2");
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateTransactionBody, InvalidOtherGroup)
{
	TransactionBody body(createdAt, GRADIDO_TRANSACTION_BODY_VERSION_STRING, CrossGroupType::LOCAL, "<script>");
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}