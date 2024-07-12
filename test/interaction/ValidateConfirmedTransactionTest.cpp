#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

TEST(ValidateConfirmedTransactionTest, validCommunityRootGradidoTransaction) {
	auto transaction = make_shared<GradidoTransaction>();
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);

	ConfirmedTransaction confirmedTransaction(
		7,
		transaction,
		confirmedAt,
		VERSION_STRING,
		nullptr,
		make_shared<memory::Block>(32),
		"899.748379"
	);
	confirmedTransaction.runningHash = std::make_shared<memory::Block>(confirmedTransaction.calculateRunningHash());

	validate::Context c(confirmedTransaction);
	EXPECT_NO_THROW(c.run());
}

TEST(ValidateConfirmedTransactionTest, invalidWrongVersion) {
	auto transaction = make_shared<GradidoTransaction>();
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);

	ConfirmedTransaction confirmedTransaction(
		7,
		transaction,
		confirmedAt,
		"1",
		nullptr,
		make_shared<memory::Block>(32),
		"899.748379"
	);
	confirmedTransaction.runningHash = std::make_shared<memory::Block>(confirmedTransaction.calculateRunningHash());

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateConfirmedTransactionTest, invalidMessageID) {
	auto transaction = make_shared<GradidoTransaction>();
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);

	ConfirmedTransaction confirmedTransaction(
		7,
		transaction,
		confirmedAt,
		VERSION_STRING,
		nullptr,
		make_shared<memory::Block>(10),
		"899.748379"
	);
	confirmedTransaction.runningHash = std::make_shared<memory::Block>(confirmedTransaction.calculateRunningHash());

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}

TEST(ValidateConfirmedTransactionTest, invalidConfirmedBeforeCreated) {
	auto transaction = make_shared<GradidoTransaction>();
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);

	ConfirmedTransaction confirmedTransaction(
		7,
		transaction,
		createdAt - chrono::seconds{1},
		VERSION_STRING,
		nullptr,
		make_shared<memory::Block>(32),
		"899.748379"
	);
	confirmedTransaction.runningHash = std::make_shared<memory::Block>(confirmedTransaction.calculateRunningHash());

	validate::Context c(confirmedTransaction);
	EXPECT_THROW(c.run(), validate::TransactionValidationInvalidInputException);
}