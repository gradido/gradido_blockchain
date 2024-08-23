
#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "KeyPairs.h"

using namespace gradido;
using namespace gradido::data;
using namespace std;
using namespace std::chrono;

#define VERSION_STRING "3.3"

TEST(GradidoTransactionBuilderTest, SignTransaction) {	
	auto body = make_unique<TransactionBody>(
		"",
		std::chrono::system_clock::from_time_t(1609459200),
		VERSION_STRING
	);
	GradidoTransactionBuilder builder;
	auto keyPair = make_shared<KeyPairEd25519>(g_KeyPairs[0].publicKey, g_KeyPairs[0].privateKey);
	auto gradidoTransaction = builder
		.setTransactionBody(std::move(body))
		.sign(keyPair)
		.build();

	ASSERT_EQ(gradidoTransaction->getBodyBytes()->convertToBase64(),
		"CgASCAiAzLn/BRAAGgMzLjMgAA=="
	);
	ASSERT_EQ(gradidoTransaction->getSignatureMap().getSignaturePairs().front().getSignature()->convertToHex(),
		"1ebd44c1d9e2033c0422d40157f5976f95c5c40c78058306e66185393686d5abe8140d87eb29103816f2772ead804b4a072415c5ab78f28d2aa99246a7229604"
	);

	//EXPECT_EQ(*gradidoTransaction->mBodyBytes, )
}
