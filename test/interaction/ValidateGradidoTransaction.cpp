#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;
using namespace std;

static void sign(GradidoTransaction& transaction, const KeyPair& keyPair)
{
	auto sign = make_shared<memory::Block>(crypto_sign_BYTES);
	unsigned long long actualSignLength = 0;
	crypto_sign_detached(*sign, &actualSignLength, *transaction.bodyBytes, transaction.bodyBytes->size(), *keyPair.privateKey);
	assert(actualSignLength == crypto_sign_BYTES);
	transaction.signatureMap.push({ keyPair.publicKey, sign });
}

TEST(ValidateGradidoTransaction, invalidBody) {
	GradidoTransaction transaction;
	transaction.bodyBytes = make_shared<memory::Block>(
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson "
	);

	validate::Context c(transaction);
	EXPECT_THROW(c.run(), GradidoNullPointerException);
}

TEST(ValidateGradidoTransaction, validCommunityRootGradidoTransaction) {
	GradidoTransaction transaction;
	// keyPair 0 is public of this community root Transaction
	transaction.bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(transaction, g_KeyPairs[0]);

	validate::Context c(transaction);
	EXPECT_NO_THROW(c.run());
}

// community root transaction must be signed by community root key pair
TEST(ValidateGradidoTransaction, invalidCommunityRootMissingSigner) {
	GradidoTransaction transaction;
	// keyPair 0 is public of this community root Transaction
	transaction.bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	validate::Context c(transaction);
	// no signature at all
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);

	sign(transaction, g_KeyPairs[1]);
	// wrong signer
	EXPECT_THROW(c.run(), validate::TransactionValidationRequiredSignMissingException);
}

// creation transaction must be signed by another user
TEST(ValidateGradidoTransaction, invalidGradidoCreationMissingSigner) {
	GradidoTransaction transaction;
	transaction.bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"ChlEZWluZSBlcnN0ZSBTY2hvZXBmdW5nIDspEggIgMy5/wUQABoDMy4zIAA6NAoqCiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VxIEMTAwMBoAGgYIuMq5/wU="
	));

	validate::Context c(transaction);
	// no signature at all
	EXPECT_THROW(c.run(), validate::TransactionValidationMissingSignException);

	sign(transaction, g_KeyPairs[4]);
	// wrong signer
	EXPECT_THROW(c.run(), validate::TransactionValidationForbiddenSignException);
}
