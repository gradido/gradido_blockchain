
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
	auto gradidoTransaction = builder
		.setTransactionBody(std::move(body))
		.sign(g_KeyPairs[0])
		.build();

	ASSERT_EQ(gradidoTransaction->getBodyBytes()->convertToBase64(),
		"CgASCAiAzLn/BRAAGgMzLjMgAA=="
	);
	ASSERT_EQ(gradidoTransaction->getSignatureMap().getSignaturePairs().front().getSignature()->convertToHex(),
		"04e0d0f6c4bbd2d87dc879fc5f72be48dbf682c888757fd5d3d6da0af4026fecc25edf7aeba19ced1e5f481d2619d4d62bf3bc357e93a053dde942a05584e400"
	);

	//EXPECT_EQ(*gradidoTransaction->mBodyBytes, )
}
