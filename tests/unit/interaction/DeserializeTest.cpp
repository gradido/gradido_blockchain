#include "gtest/gtest.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "../KeyPairs.h"
#include "../serializedTransactions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;

TEST(DeserializeTest, HieroAccountId)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64("CAAQABjvpQE="));
	deserialize::Context context(rawData, deserialize::Type::HIERO_ACCOUNT_ID);
	context.run();

	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());
	ASSERT_TRUE(context.isHieroAccountId());

	EXPECT_EQ(context.getHieroAccountId().getAccountNum(), 21231);
}

TEST(DeserializeTest, HieroTopicId)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64("CAAQABjVDA=="));
	deserialize::Context context(rawData, deserialize::Type::HIERO_TOPIC_ID);
	context.run();

	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());
	ASSERT_TRUE(context.isHieroTopicId());

	EXPECT_EQ(context.getHieroTopicId().getTopicNum(), 1621);
}

TEST(DeserializeTest, HieroTransactionId)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(hieroTransactionIdBase64));
	deserialize::Context context(rawData, deserialize::Type::HIERO_TRANSACTION_ID);
	context.run();

	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());
	EXPECT_FALSE(context.isHieroAccountId());
	ASSERT_TRUE(context.isHieroTransactionId());

	EXPECT_EQ(context.getHieroTransactionId().toString(), "0.0.121212@172618921.000029182");
}



TEST(DeserializeTest, CommunityRootBody)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), GRDT_CROSS_GROUP_LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	ASSERT_TRUE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	const auto& dict = g_appContext->getCommunityContext(communityIdIndex).getBlockchain()->getPublicKeyDictionary();
	const auto& communityRoot = body->getCommunityRoot();
	const auto& communityRootPublicKey = dict.getDataForIndexOrThrow(communityRoot->publicKeyIndex);
	const auto& communityRootGmwPubkey = dict.getDataForIndexOrThrow(communityRoot->gmwPublicKeyIndex);
	const auto& communityRootAufPubkey = dict.getDataForIndexOrThrow(communityRoot->aufPublicKeyIndex);
	EXPECT_TRUE(communityRootPublicKey.isTheSame(g_KeyPairs[0]->getPublicKey()->data()));
	EXPECT_TRUE(communityRootGmwPubkey.isTheSame(g_KeyPairs[1]->getPublicKey()->data()));
	EXPECT_TRUE(communityRootAufPubkey.isTheSame(g_KeyPairs[2]->getPublicKey()->data()));
	
}


TEST(DeserializeTest, RegisterAddressBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(registeAddressTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), GRDT_CROSS_GROUP_LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	ASSERT_TRUE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto registerAddress = body->getRegisterAddress();
	EXPECT_EQ(registerAddress->addressType, GRDT_ADDRESS_COMMUNITY_HUMAN);
	EXPECT_EQ(registerAddress->derivationIndex, 1);
	
	const auto& dict = g_appContext->getCommunityContext(communityIdIndex).getBlockchain()->getPublicKeyDictionary();
	const auto& userPublicKeyIndex = dict.getDataForIndexOrThrow(registerAddress->userPublicKeyIndex);
	const auto& accountPublicKeyIndex = dict.getDataForIndexOrThrow(registerAddress->accountPublicKeyIndex);
	EXPECT_TRUE(g_KeyPairs[3]->getPublicKey()->isTheSame(userPublicKeyIndex));
	// printf("name hash: %s\n", g_appContext->getUserNameHashs().getDataForIndex(registerAddress->nameHashIndex)->convertToHex().c_str());
	// EXPECT_FALSE(registerAddress->getNameHash());
	EXPECT_TRUE(g_KeyPairs[4]->getPublicKey()->isTheSame(accountPublicKeyIndex));
}


TEST(DeserializeTest, GradidoCreationBody) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(creationTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), GRDT_CROSS_GROUP_LOCAL);
	auto& memos = body->getMemos();
	ASSERT_GE(memos.size(), 1);
	EXPECT_EQ(memos[0].getMemo().copyAsString(), std::string("Deine erste Schoepfung ;)"));

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	ASSERT_TRUE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto creation = body->getCreation();
	auto& recipient = creation->getRecipient();
	EXPECT_EQ(recipient.getAmount(), GradidoUnit::fromGradidoCent(10000000));
	EXPECT_TRUE(recipient.getPublicKey()->isTheSame(g_KeyPairs[4]->getPublicKey()));
	EXPECT_EQ(creation->getTargetDate(), targetDate);
}



TEST(DeserializeTest, GradidoTransferBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(transferTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), GRDT_CROSS_GROUP_LOCAL);
	auto& memos = body->getMemos();
	ASSERT_GE(memos.size(), 1);
	EXPECT_EQ(memos[0].getMemo().copyAsString(), std::string("Ich teile mit dir"));

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	ASSERT_TRUE(body->isTransfer());

	auto transfer = body->getTransfer();
	auto& sender = transfer->getSender();
	EXPECT_EQ(sender.getAmount(), GradidoUnit::fromGradidoCent(5005500));
	EXPECT_TRUE(sender.getPublicKey()->isTheSame(g_KeyPairs[4]->getPublicKey()));
	EXPECT_TRUE(transfer->getRecipient()->isTheSame(g_KeyPairs[5]->getPublicKey()));
}


TEST(DeserializeTest, GradidoDeferredTransferBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(deferredTransferTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), GRDT_CROSS_GROUP_LOCAL);
	auto& memos = body->getMemos();
	ASSERT_GE(memos.size(), 1);
	EXPECT_EQ(memos[0].getMemo().copyAsString(), std::string("Link zum einloesen"));
	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	ASSERT_TRUE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto deferredTransfer = body->getDeferredTransfer();
	auto& transfer = deferredTransfer->getTransfer();
	auto& sender = transfer.getSender();
	EXPECT_EQ(sender.getAmount(), GradidoUnit::fromGradidoCent(5555500));
	EXPECT_TRUE(sender.getPublicKey()->isTheSame(g_KeyPairs[4]->getPublicKey()));
	EXPECT_TRUE(transfer.getRecipient()->isTheSame(g_KeyPairs[5]->getPublicKey()));
	EXPECT_EQ(deferredTransfer->getTimeoutDuration(), timeoutDuration);
}


TEST(DeserializeTest, CommunityFriendsUpdateBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(communityFriendsUpdateBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), GRDT_CROSS_GROUP_LOCAL);

	ASSERT_TRUE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto communityFriends = body->getCommunityFriendsUpdate();
	EXPECT_TRUE(communityFriends->getColorFusion());
}


TEST(DeserializeTest, GradidoTransaction) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(gradidoTransactionSignedInvalidBody));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto gradidoTransaction = context.getGradidoTransaction();
	auto bodyBytes = memory::Block(invalidBodyTestPayload);
	//printf("signature: %s\n", gradidoTransaction->signatureMap.signaturePairs.front().signature->convertToHex().data());
	EXPECT_TRUE(gradidoTransaction->getBodyBytes()->isTheSame(bodyBytes));
	auto& firstSignaturePair = gradidoTransaction->getSignatureMap().getSignaturePairs().front();
	EXPECT_EQ(crypto_sign_verify_detached(
		*firstSignaturePair.getSignature(),
		bodyBytes,
		bodyBytes.size(),
		*g_KeyPairs[3]->getPublicKey()
	), 0);
}


TEST(DeserializeTest, CompleteConfirmedTransaction) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(completeConfirmedTransaction));
	deserialize::Context context(rawData, deserialize::Type::CONFIRMED_TRANSACTION);
	context.run(communityIdIndex);
	EXPECT_FALSE(context.isTransactionBody());
	ASSERT_TRUE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto confirmedTransaction = context.getConfirmedTransaction();

	EXPECT_EQ(confirmedTransaction->getId(), 7);
	EXPECT_EQ(confirmedTransaction->getConfirmedAt(), confirmedAt);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[4]->getPublicKey(), communityIdIndex).getBalance(), GradidoUnit::fromGradidoCent(1000000));
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[5]->getPublicKey(), communityIdIndex).getBalance(), GradidoUnit::fromGradidoCent(8997483));
	ASSERT_EQ(confirmedTransaction->getRunningHash()->size(), crypto_generichash_BYTES);
	EXPECT_EQ(confirmedTransaction->getRunningHash()->convertToHex(), "0000000000000000000000000000000000000000000000000000000000000000");

	auto gradidoTransaction = confirmedTransaction->getGradidoTransaction();
	auto firstSignature = gradidoTransaction->getSignatureMap().getSignaturePairs().front().getSignature();
	auto bodyBytes = gradidoTransaction->getBodyBytes();
	EXPECT_TRUE(g_KeyPairs[0]->verify(*bodyBytes, *firstSignature));
	EXPECT_FALSE(g_KeyPairs[2]->verify(*bodyBytes, *firstSignature));

	deserialize::Context secondContext(bodyBytes);
	secondContext.run(communityIdIndex);

	ASSERT_TRUE(secondContext.isTransactionBody());
	EXPECT_FALSE(secondContext.isConfirmedTransaction());
	EXPECT_FALSE(secondContext.isGradidoTransaction());

	auto body = secondContext.getTransactionBody();
	auto& memos = body->getMemos();
	ASSERT_GE(memos.size(), 1);
	EXPECT_EQ(memos[0].getMemo().copyAsString(), completeTransactionMemoString);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_TRUE(body->isTransfer());

	auto transfer = body->getTransfer();
	auto& sender = transfer->getSender();
	EXPECT_EQ(sender.getAmount(), GradidoUnit::fromGradidoCent(1002516));
	EXPECT_TRUE(sender.getPublicKey()->isTheSame(g_KeyPairs[4]->getPublicKey()));
	EXPECT_TRUE(transfer->getRecipient()->isTheSame(g_KeyPairs[5]->getPublicKey()));
}


TEST(DeserializeTest, TransactionTriggerEvent) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64("CAwSCAjC8rn/BRAAGAE="));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_TRIGGER_EVENT);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	ASSERT_TRUE(context.isTransactionTriggerEvent());

	auto transactionTriggerEvent = context.getTransactionTriggerEvent();
	EXPECT_EQ(transactionTriggerEvent.getLinkedTransactionId(), 12);
	EXPECT_EQ(transactionTriggerEvent.getTargetDate(), confirmedAt);
	EXPECT_EQ(transactionTriggerEvent.getType(), data::TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL);
}
