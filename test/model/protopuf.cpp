#include "protopuf.h"

//#include "gradido_blockchain/model/protopufTypes.h"
//#include "gradido_blockchain/model/TransactionFactory.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

void protopuf::SetUp()
{
	CryptoConfig::loadMnemonicWordLists();
	for (auto i = 0; i < 6; i++) {
		auto passphrase = Passphrase::generate(&CryptoConfig::g_Mnemonic_WordLists[2]);
		mTestKeyPairs.push_back(KeyPairEd25519::create(passphrase));
	}
	
}

void protopuf::TearDown()
{

}

TEST_F(protopuf, CommunityRoot) {
	/*
	auto mm = MemoryManager::getInstance();
	auto pubkey = mTestKeyPairs[0]->getPublicKeyCopy();
	auto gmwPubkey = mTestKeyPairs[1]->getPublicKeyCopy();
	auto aufPubkey = mTestKeyPairs[1]->getPublicKeyCopy();
	auto gradidoTransaction = TransactionFactory::createCommunityRoot(pubkey, gmwPubkey, aufPubkey);
	auto bodyBytes = gradidoTransaction->getTransactionBody()->getBodyBytes();
	auto signature = mTestKeyPairs[0]->sign(*bodyBytes.get());
	gradidoTransaction->addSign(pubkey, signature);
	auto serializedTransaction = gradidoTransaction->getSerialized();

	std::span<std::byte> gradidoTransacionByteSpan(reinterpret_cast<std::byte*>(serializedTransaction->data()), serializedTransaction->size());
	auto [protopufGradidoTransaction, bufferEnd2] = message_coder<model::protopuf::GradidoTransaction>::decode(gradidoTransacionByteSpan);
	model::protopuf::SignatureMap sigMap = protopufGradidoTransaction["sig_map"_f].value();
	auto firstSigPair = sigMap["sig_pair"_f][0];
	std::vector<unsigned char>& firstPubkey = firstSigPair["pubkey"_f].value();
	std::vector<unsigned char>& firstSignature = firstSigPair["signature"_f].value();
	EXPECT_EQ(firstPubkey.size(), pubkey->size());
	EXPECT_FALSE(memcmp(firstPubkey.data(), pubkey->data(), firstPubkey.size()));
	EXPECT_EQ(firstSignature.size(), signature->size());	
	EXPECT_FALSE(memcmp(firstSignature.data(), signature->data(), firstSignature.size()));

	std::span<std::byte> bodyBytesSpan(reinterpret_cast<std::byte*>(bodyBytes->data()), bodyBytes->size());
	auto [protopufBodyBytes, bufferEnd3] = message_coder<model::protopuf::TransactionBody>::decode(bodyBytesSpan);
	EXPECT_TRUE(protopufBodyBytes["community_root"_f].has_value());
	model::protopuf::CommunityRoot communityRoot = protopufBodyBytes["community_root"_f].value();
	std::vector<unsigned char>& crPubkey = communityRoot["pubkey"_f].value();
	EXPECT_EQ(crPubkey.size(), pubkey->size());
	EXPECT_FALSE(memcmp(crPubkey.data(), pubkey->data(), crPubkey.size()));
	EXPECT_FALSE(protopufBodyBytes["transfer"_f].has_value());
	
	
	// std::string publicKey = protopufGradidoTransaction["sig_map"_f]["sig_pair"_f][0]["pubkey"_f];
	//printf("size: %d\n", (int)publicKey.size());
	//for (const auto& signaturePair : sigMap["sig_pair"_f]) {
	//		auto pubkey = signaturePair["pubkey"_f];
	//		auto signature = signaturePair["signature"_f];
	//}
	
	mm->releaseMemory(signature);
	mm->releaseMemory(pubkey);	
	mm->releaseMemory(gmwPubkey);
	mm->releaseMemory(aufPubkey);
	*/
}