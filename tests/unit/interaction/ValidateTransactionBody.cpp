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
	TransactionBody body(createdAt, 0);
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}


TEST(ValidateTransactionBody, InvalidOtherGroup)
{
	TransactionBody body(createdAt, 0, CrossGroupType::LOCAL, 7);
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}