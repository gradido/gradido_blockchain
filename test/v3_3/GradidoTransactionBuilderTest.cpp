#include "gtest/gtest.h"
#include "gradido_blockchain/v3_3/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"

using namespace gradido::v3_3;
using namespace gradido::v3_3::data;
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

	ASSERT_EQ(DataTypeConverter::binToBase64(*gradidoTransaction->mBodyBytes),
		"CgASCAiAzLn/BRAAGgMzLjMgAA=="
	);
	ASSERT_EQ(gradidoTransaction->mSignatureMap.signaturePairs.front().signature->convertToHex(), 
		"1ebd44c1d9e2033c0422d40157f5976f95c5c40c78058306e66185393686d5abe8140d87eb29103816f2772ead804b4a072415c5ab78f28d2aa99246a7229604"
	);

	//EXPECT_EQ(*gradidoTransaction->mBodyBytes, )
}
