#include "gtest/gtest.h"
#include "../KeyPairs.h"
#include "gradido_blockchain/model/TransactionFactory.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionBody.h"
#include "gradido_blockchain/MemoryManager.h"
#include "proto/gradido/transaction_body.pb.h"

#define VERSION_STRING "3.3"
const auto createdAt = std::chrono::system_clock::from_time_t(1609459200); //2021-01-01 00:00:00 UTC

TEST(SerializeTest, TransactionBodyWithoutMemo)
{
	proto::gradido::TransactionBody body;//("", createdAt, VERSION_STRING);
  body.set_version_number(VERSION_STRING);
  body.mutable_created_at()->set_seconds(1609459200);
  body.set_type(proto::gradido::TransactionBody_CrossGroupType_LOCAL);
  auto serialized = body.SerializeAsString();
  EXPECT_TRUE(body.ParseFromString(DataTypeConverter::base64ToBinString("CgASCAiAzLn/BRAAGgMzLjMgAA==")));
  EXPECT_EQ(body.version_number(), VERSION_STRING);
  EXPECT_EQ(body.created_at().seconds(), 1609459200);
  EXPECT_EQ(body.created_at().nanos(), 0);
  EXPECT_EQ(body.type(), proto::gradido::TransactionBody_CrossGroupType_LOCAL);


	printf("serialized size: %d, serialized in base64: %s\n", serialized.size(), DataTypeConverter::binToBase64(serialized).data());
  printf("hex: %s\n", DataTypeConverter::binToHex(serialized).data());
}

TEST(SerializeTest, TransactionBodyWithoutMemoFriendsUpdate)
{
	proto::gradido::TransactionBody body;//("", createdAt, VERSION_STRING);
  body.set_version_number(VERSION_STRING);
  body.mutable_created_at()->set_seconds(1609459200);
  body.set_type(proto::gradido::TransactionBody_CrossGroupType_LOCAL);
  body.mutable_community_friends_update()->set_color_fusion(true);
  auto serialized = body.SerializeAsString();
  EXPECT_TRUE(body.ParseFromString(DataTypeConverter::base64ToBinString("CgASCAiAzLn/BRAAGgMzLjMgAA==")));
  EXPECT_EQ(body.version_number(), VERSION_STRING);
  EXPECT_EQ(body.created_at().seconds(), 1609459200);
  EXPECT_EQ(body.created_at().nanos(), 0);
  EXPECT_EQ(body.type(), proto::gradido::TransactionBody_CrossGroupType_LOCAL);


	printf("serialized size: %d, serialized in base64: %s\n", serialized.size(), DataTypeConverter::binToBase64(serialized).data());
  printf("hex: %s\n", DataTypeConverter::binToHex(serialized).data());
}


TEST(GradidoTransactionTest, CommunityRoot)
{
  printf("pubkey: %s\n", g_KeyPairs[0]->publicKey->convertToHexString().data());
  printf("gmw key: %s\n", g_KeyPairs[1]->publicKey->convertToHexString().data());
  printf("auf key: %s\n", g_KeyPairs[2]->publicKey->convertToHexString().data());
	auto gradidoTransaction = TransactionFactory::createCommunityRoot(g_KeyPairs[0]->publicKey, g_KeyPairs[1]->publicKey, g_KeyPairs[2]->publicKey);
	gradidoTransaction->setCreatedAt(createdAt);
	auto base64 = DataTypeConverter::binToBase64(std::move(gradidoTransaction->getTransactionBody()->getBodyBytes()));
	printf("community root: %s\n", base64->c_str());
}

TEST(GradidoTransactionTest, CommunityRootFromBase64)
{
  auto transactionBodyBase64 = "CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hogUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa";
  auto body = model::gradido::TransactionBody::load(DataTypeConverter::base64ToBinString(transactionBodyBase64), ProtobufArenaMemory::create());
  EXPECT_TRUE(body->isCommunityRoot());
  auto communityRoot = body->getCommunityRoot();
  /*EXPECT_TRUE(communityRoot->getPubkey()->isSame(g_KeyPairs[0]->publicKey));
  EXPECT_TRUE(communityRoot->getGMWPubkey()->isSame(g_KeyPairs[1]->publicKey));
  EXPECT_TRUE(communityRoot->getAUFPubkey()->isSame(g_KeyPairs[2]->publicKey));
  */
  
  printf("pubkey: %s\n", communityRoot->getPubkey()->convertToHexString().data());
  printf("gmw key: %s\n", communityRoot->getGMWPubkey()->convertToHexString().data());
  printf("auf key: %s\n", communityRoot->getAUFPubkey()->convertToHexString().data());
  
}