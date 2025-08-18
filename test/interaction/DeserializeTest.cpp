#include "gtest/gtest.h"
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

TEST(DeserializeTest, HieroTransactionId)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64("CgkIqemnUhD+4wESCAgAEAAY/LIHGAEgeQ=="));
	deserialize::Context context(rawData, deserialize::Type::HIERO_TRANSACTION_ID);
	context.run();

	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());
	EXPECT_FALSE(context.isHieroAccountId());
	ASSERT_TRUE(context.isHieroTransactionId());

	EXPECT_EQ(context.getHieroTransactionId().toString(), "0.0.121212@172618921.29182");
}



TEST(DeserializeTest, CommunityRootBody)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(communityRootTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	ASSERT_TRUE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto communityRoot = body->getCommunityRoot();
	EXPECT_TRUE(communityRoot->getPublicKey()->isTheSame(g_KeyPairs[0]->getPublicKey()));
	EXPECT_TRUE(communityRoot->getGmwPubkey()->isTheSame(g_KeyPairs[1]->getPublicKey()));
	EXPECT_TRUE(communityRoot->getAufPubkey()->isTheSame(g_KeyPairs[2]->getPublicKey()));
}


TEST(DeserializeTest, RegisterAddressBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(registeAddressTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	ASSERT_TRUE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto registerAddress = body->getRegisterAddress();
	EXPECT_EQ(registerAddress->getAddressType(), AddressType::COMMUNITY_HUMAN);
	EXPECT_EQ(registerAddress->getDerivationIndex(), 1);
	EXPECT_TRUE(registerAddress->getUserPublicKey()->isTheSame(g_KeyPairs[3]->getPublicKey()));
	EXPECT_FALSE(registerAddress->getNameHash());
	EXPECT_TRUE(registerAddress->getAccountPublicKey()->isTheSame(g_KeyPairs[4]->getPublicKey()));
}


TEST(DeserializeTest, GradidoCreationBody) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(creationTransactionBase64));
	deserialize::Context context(rawData, deserialize::Type::GRADIDO_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), CrossGroupType::LOCAL);
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
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), CrossGroupType::LOCAL);
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
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), CrossGroupType::LOCAL);
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
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto transaction = context.getGradidoTransaction();
	ASSERT_TRUE(transaction);
	auto body = transaction->getTransactionBody();
	ASSERT_TRUE(body);
	EXPECT_EQ(body->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(body->getCreatedAt(), createdAt);
	EXPECT_EQ(body->getType(), CrossGroupType::LOCAL);

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
	context.run();
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


TEST(DeserializeTest, MinimalConfirmedTransaction) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(minimalConfirmedTransaction));
	deserialize::Context context(rawData, deserialize::Type::CONFIRMED_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	ASSERT_TRUE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto confirmedTransaction = context.getConfirmedTransaction();
	auto gradidoTransaction = confirmedTransaction->getGradidoTransaction();

	EXPECT_EQ(confirmedTransaction->getId(), 7);
	EXPECT_EQ(confirmedTransaction->getConfirmedAt(), confirmedAt);
	EXPECT_EQ(confirmedTransaction->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(confirmedTransaction->getAccountBalances().size(), 0);
	EXPECT_EQ(confirmedTransaction->getRunningHash()->size(), crypto_generichash_BYTES);
}


TEST(DeserializeTest, CompleteConfirmedTransaction) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(completeConfirmedTransaction));
	deserialize::Context context(rawData, deserialize::Type::CONFIRMED_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	ASSERT_TRUE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());
	EXPECT_FALSE(context.isTransactionTriggerEvent());

	auto confirmedTransaction = context.getConfirmedTransaction();

	EXPECT_EQ(confirmedTransaction->getId(), 7);
	EXPECT_EQ(confirmedTransaction->getConfirmedAt(), confirmedAt);
	EXPECT_EQ(confirmedTransaction->getVersionNumber(), VERSION_STRING);
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[4]->getPublicKey()).getBalance(), GradidoUnit::fromGradidoCent(1000000));
	EXPECT_EQ(confirmedTransaction->getAccountBalance(g_KeyPairs[5]->getPublicKey()).getBalance(), GradidoUnit::fromGradidoCent(8997483));
	ASSERT_EQ(confirmedTransaction->getRunningHash()->size(), crypto_generichash_BYTES);
	EXPECT_EQ(confirmedTransaction->getRunningHash()->convertToHex(), "1203c5aa94a724a49f10d00db79b8261e3fcb210588087d4a696a99c7a6c7103");

	auto gradidoTransaction = confirmedTransaction->getGradidoTransaction();
	auto firstSignature = gradidoTransaction->getSignatureMap().getSignaturePairs().front().getSignature();
	auto bodyBytes = gradidoTransaction->getBodyBytes();
	EXPECT_TRUE(g_KeyPairs[0]->verify(*bodyBytes, *firstSignature));
	EXPECT_FALSE(g_KeyPairs[2]->verify(*bodyBytes, *firstSignature));

	deserialize::Context secondContext(bodyBytes);
	secondContext.run();

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
