#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "gradido_blockchain/model/TransactionFactory.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

const auto createdAt = 1609459200; //2021-01-01 00:00:00 UTC

TEST(GradidoTransactionTest, CommunityRoot)
{
	auto gradidoTransaction = TransactionFactory::createCommunityRoot(g_KeyPairs[0]->publicKey, g_KeyPairs[1]->publicKey, g_KeyPairs[2]->publicKey);
	gradidoTransaction->setCreatedAt(1609459200);
	auto base64 = DataTypeConverter::binToBase64(std::move(gradidoTransaction->getSerialized()));
	printf("community root: %s\n", base64->c_str());
}