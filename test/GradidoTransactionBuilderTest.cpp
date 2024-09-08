
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
		"2d9ab75e055a4853ce3cf69b8a121b052b32a50332b87ee540c6e1cfd5914ef9e485366d779478f104a39eee79d907b3a9aae64b95878c812562b2cd4ae71504"
	);

	//EXPECT_EQ(*gradidoTransaction->mBodyBytes, )
}
