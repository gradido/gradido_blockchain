#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "../KeyPairs.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;


TEST(DeserializeTest, TransactionBodyWithoutMemo)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64("CgASCAiAzLn/BRAAGgMzLjMgAA=="));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());
}


TEST(DeserializeTest, CommunityRootBody)
{
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	ASSERT_TRUE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto communityRoot = body->communityRoot;
	EXPECT_TRUE(communityRoot->pubkey->isTheSame(g_KeyPairs[0].publicKey));
	EXPECT_TRUE(communityRoot->gmwPubkey->isTheSame(g_KeyPairs[1].publicKey));
	EXPECT_TRUE(communityRoot->aufPubkey->isTheSame(g_KeyPairs[2].publicKey));
}


TEST(DeserializeTest, RegisterAddressBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAEpICiAllxqg50IhRNzCRIh+Ke8WDVR5sSGemBfKbs44sJ83wBABIiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VygB"
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	ASSERT_TRUE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto registerAddress = body->registerAddress;
	EXPECT_EQ(registerAddress->addressType, AddressType::COMMUNITY_HUMAN);
	EXPECT_EQ(registerAddress->derivationIndex, 1);
	EXPECT_TRUE(registerAddress->userPubkey->isTheSame(g_KeyPairs[3].publicKey));
	EXPECT_FALSE(registerAddress->nameHash);
	EXPECT_TRUE(registerAddress->accountPubkey->isTheSame(g_KeyPairs[4].publicKey));
}


TEST(DeserializeTest, GradidoCreationBody) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"ChlEZWluZSBlcnN0ZSBTY2hvZXBmdW5nIDspEggIgMy5/wUQABoDMy4zIAA6NAoqCiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VxIEMTAwMBoAGgYIuMq5/wU="
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);
	EXPECT_EQ(body->memo, std::string("Deine erste Schoepfung ;)"));

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	ASSERT_TRUE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto creation = body->creation;
	EXPECT_EQ(creation->recipient.mAmount.toString(), "1000.0000");
	EXPECT_TRUE(creation->recipient.mPubkey->isTheSame(g_KeyPairs[4].publicKey));
	EXPECT_EQ(creation->targetDate, targetDate);
}



TEST(DeserializeTest, GradidoTransferBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"ChFJY2ggdGVpbGUgbWl0IGRpchIICIDMuf8FEAAaAzMuMyAAMlAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjUwMC41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7Fdg=="
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);
	EXPECT_EQ(body->memo, std::string("Ich teile mit dir"));

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	ASSERT_TRUE(body->isTransfer());

	auto transfer = body->transfer;
	EXPECT_EQ(transfer->sender.mAmount.toString(), "500.5500");
	EXPECT_TRUE(transfer->sender.mPubkey->isTheSame(g_KeyPairs[4].publicKey));
	EXPECT_TRUE(transfer->recipient->isTheSame(g_KeyPairs[5].publicKey));
}


TEST(DeserializeTest, GradidoDeferredTransferBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"ChJMaW5rIHp1bSBlaW5sb2VzZW4SCAiAzLn/BRAAGgMzLjMgAFJaClAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjU1NS41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhIGCKj5uf8F"
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);
	EXPECT_EQ(body->memo, std::string("Link zum einloesen"));

	EXPECT_FALSE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	ASSERT_TRUE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto deferredTransfer = body->deferredTransfer;
	auto transfer = deferredTransfer->transfer;
	EXPECT_EQ(transfer.sender.mAmount.toString(), "555.5500");
	EXPECT_TRUE(transfer.sender.mPubkey->isTheSame(g_KeyPairs[4].publicKey));
	EXPECT_TRUE(transfer.recipient->isTheSame(g_KeyPairs[5].publicKey));
	EXPECT_EQ(deferredTransfer->timeout, timeout);
}


TEST(DeserializeTest, CommunityFriendsUpdateBody) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAEICCAE="
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	ASSERT_TRUE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto body = context.getTransactionBody();
	EXPECT_EQ(body->versionNumber, VERSION_STRING);
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_EQ(body->type, CrossGroupType::LOCAL);

	ASSERT_TRUE(body->isCommunityFriendsUpdate());
	EXPECT_FALSE(body->isCommunityRoot());
	EXPECT_FALSE(body->isCreation());
	EXPECT_FALSE(body->isDeferredTransfer());
	EXPECT_FALSE(body->isRegisterAddress());
	EXPECT_FALSE(body->isTransfer());

	auto communityFriends = body->communityFriendsUpdate;
	EXPECT_TRUE(communityFriends->colorFusion);
}


TEST(DeserializeTest, GradidoTransaction) {
	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CmYKZAogJZcaoOdCIUTcwkSIfinvFg1UebEhnpgXym7OOLCfN8ASQNnYDRZmlKkhvEiea/cRiqwr2bExLlkQxcbwjDYXy3nC3Ggin7Jous8li9gLfQpnUX9FzhrbqOiMEvQqVisNHQUSigEnJ1RvIGJlIHlvdXJzZWxmIGluIGEgd29ybGQgdGhhdCBpcyBjb25zdGFudGx5IHRyeWluZyB0byBtYWtlIHlvdSBzb21ldGhpbmcgZWxzZSBpcyB0aGUgZ3JlYXRlc3QgYWNjb21wbGlzaG1lbnQuJycKIC0gUmFscGggV2FsZG8gRW1lcnNvbiA="
	));
	deserialize::Context context(rawData, deserialize::Type::TRANSACTION_BODY);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	EXPECT_FALSE(context.isConfirmedTransaction());
	ASSERT_TRUE(context.isGradidoTransaction());

	auto gradidoTransaction = context.getGradidoTransaction();
	auto bodyBytes = memory::Block(
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson "
	);
	//printf("signature: %s\n", gradidoTransaction->signatureMap.signaturePairs.front().signature->convertToHex().data());
	EXPECT_TRUE(gradidoTransaction->bodyBytes->isTheSame(bodyBytes));
	EXPECT_EQ(crypto_sign_verify_detached(
		*gradidoTransaction->signatureMap.signaturePairs.front().signature,
		bodyBytes,
		bodyBytes.size(),
		*g_KeyPairs[3].publicKey
	), 0);
}


TEST(DeserializeTest, MinimalConfirmedTransaction) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CAcSAgoAGgYIwvK5/wUiAzMuMyogAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOgMxNzk="
	));
	deserialize::Context context(rawData, deserialize::Type::CONFIRMED_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	ASSERT_TRUE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto confirmedTransaction = context.getConfirmedTransaction();
	auto gradidoTransaction = confirmedTransaction->gradidoTransaction.get();

	EXPECT_EQ(confirmedTransaction->id, 7);
	EXPECT_EQ(confirmedTransaction->confirmedAt, confirmedAt);
	EXPECT_EQ(confirmedTransaction->versionNumber, VERSION_STRING);
	EXPECT_EQ(confirmedTransaction->accountBalance.toString(), "179.0000");
	EXPECT_EQ(confirmedTransaction->runningHash->size(), crypto_generichash_BYTES);
}


TEST(DeserializeTest, CompleteConfirmedTransaction) {

	auto rawData = std::make_shared<memory::Block>(memory::Block::fromBase64(
		"CAcS5gEKZgpkCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hJABLGV+LKPuwm777Y98sSKCJjyT764T0LkK9JMAlH8McVfE4a58QJDRkRVVw8s+97770Ecu41wN699/2dl2bMyCRJ8ChVEYW5rZSBmdWVyIGRlaW4gU2VpbiESCAiAzLn/BRAAGgMzLjMgADJSCi4KIIqMkyk8uX6HhBeNqK5YgUT3yYL0ZYv9NRAaHitHnD5XEggxMDAuMjUxNhoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhoGCMLyuf8FIgMzLjMqIIECZhgxQyc+ARHQ/855AyzGFnuR8jjwBqgBYRWxbkANMiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADoIODk5Ljc0ODQ="
	));
	deserialize::Context context(rawData, deserialize::Type::CONFIRMED_TRANSACTION);
	context.run();
	EXPECT_FALSE(context.isTransactionBody());
	ASSERT_TRUE(context.isConfirmedTransaction());
	EXPECT_FALSE(context.isGradidoTransaction());

	auto confirmedTransaction = context.getConfirmedTransaction();

	EXPECT_EQ(confirmedTransaction->id, 7);
	EXPECT_EQ(confirmedTransaction->confirmedAt, confirmedAt);
	EXPECT_EQ(confirmedTransaction->versionNumber, VERSION_STRING);
	EXPECT_EQ(confirmedTransaction->accountBalance.toString(), "899.7484");
	ASSERT_EQ(confirmedTransaction->runningHash->size(), crypto_generichash_BYTES);
	EXPECT_EQ(confirmedTransaction->runningHash->convertToHex(), "810266183143273e0111d0ffce79032cc6167b91f238f006a8016115b16e400d");

	auto gradidoTransaction = confirmedTransaction->gradidoTransaction.get();
	KeyPairEd25519 keyPair(g_KeyPairs[0].publicKey, g_KeyPairs[0].privateKey);
	EXPECT_TRUE(keyPair.verify(*gradidoTransaction->bodyBytes, *gradidoTransaction->signatureMap.signaturePairs.front().signature));	
	KeyPairEd25519 keyPair2(g_KeyPairs[2].publicKey, g_KeyPairs[2].privateKey);
	EXPECT_FALSE(keyPair2.verify(*gradidoTransaction->bodyBytes, *gradidoTransaction->signatureMap.signaturePairs.front().signature));

	deserialize::Context secondContext(gradidoTransaction->bodyBytes);
	secondContext.run();

	ASSERT_TRUE(secondContext.isTransactionBody());
	EXPECT_FALSE(secondContext.isConfirmedTransaction());
	EXPECT_FALSE(secondContext.isGradidoTransaction());

	auto body = secondContext.getTransactionBody();
	EXPECT_EQ(body->memo, "Danke fuer dein Sein!");
	EXPECT_EQ(body->createdAt, createdAt);
	EXPECT_TRUE(body->isTransfer());
	
	auto transfer = body->transfer;
	EXPECT_EQ(transfer->sender.mAmount.toString(), "100.2516");
	EXPECT_TRUE(transfer->sender.mPubkey->isTheSame(g_KeyPairs[4].publicKey));
	EXPECT_TRUE(transfer->recipient->isTheSame(g_KeyPairs[5].publicKey));
	
}