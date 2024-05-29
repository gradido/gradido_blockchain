#include "gtest/gtest.h"
#include "protopuf/message.h"
#include "gradido_blockchain/model/protopuf/CommunityRoot.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/crypto/Passphrase.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "loguru/loguru.hpp"

using namespace pp;
using namespace model::protopuf;

TEST(modelProtopufCommunityRoot, protobufEncodeProtopufDecode) {
	
	CryptoConfig::loadMnemonicWordLists();
	std::vector<std::unique_ptr< KeyPairEd25519>> testKeyPairs;
	for (auto i = 0; i < 6; i++) {
		auto passphrase = Passphrase::generate(&CryptoConfig::g_Mnemonic_WordLists[2]);
		testKeyPairs.push_back(KeyPairEd25519::create(passphrase));
	}
	for (const auto& keyPair : testKeyPairs) {
		LOG_F(INFO, "pubkey: %s", keyPair->getPublicKeyHex().data());
	}
	CachedMemoryBlock buffer(20);
	CommunityRoot communityRoot(
		std::make_shared<CachedMemoryBlock>(testKeyPairs[0]->getPublicKeySize(), testKeyPairs[0]->getPublicKey()),
		std::make_shared<CachedMemoryBlock>(testKeyPairs[1]->getPublicKeySize(), testKeyPairs[0]->getPublicKey()),
		std::make_shared<CachedMemoryBlock>(testKeyPairs[2]->getPublicKeySize(), testKeyPairs[0]->getPublicKey())
	);
	message_coder<CommunityRootMessage>::encode()
}