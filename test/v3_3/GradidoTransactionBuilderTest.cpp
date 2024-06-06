#include "gtest/gtest.h"
#include "gradido_blockchain/v3_3/GradidoTransactionBuilder.h"
#include "../KeyPairs.h"

using namespace gradido::v3_3;
using namespace gradido::v3_3::data;
using namespace std;
using namespace std::chrono;

TEST(GradidoTransactionBuilderTest, SignTransaction) {
	auto now = system_clock::now();
	
	std::string memo("Die Welt und ein Paar Schlittschuhe dazu.");
	TransactionBodyBuilder bodyBuilder;
	auto transactionBody = bodyBuilder
		.setCommunityRoot(g_KeyPairs[0].publicKey, g_KeyPairs[1].publicKey, g_KeyPairs[2].publicKey)
		.setMemo(memo)
		.setCreatedAt(std::chrono::system_clock::from_time_t(1609459200)) //2021-01-01 00:00:00 UTC
		.build();
	printf("body builded\n");
	GradidoTransactionBuilder builder;
	auto keyPair = make_shared<KeyPairEd25519>(g_KeyPairs[0].publicKey, g_KeyPairs[1].privateKey);
	auto gradidoTransaction = builder
		.setTransactionBody(std::move(transactionBody))
		.sign(keyPair)
		.build();

	printf("bodybytes: %s\n", DataTypeConverter::binToBase64(*gradidoTransaction->mBodyBytes).data());

	//EXPECT_EQ(*gradidoTransaction->mBodyBytes, )
}
