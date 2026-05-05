#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/memory/Block.h"
#include "gtest/gtest.h"

using gradido::data::EncryptedMemo;
using memory::Block;

const char* testMemo = "Monthly community contribution to support local food sharing and strengthen regional resilience.";
const char* seed1 = "Gradido grows where generosity flows freely and every act of giving multiplies shared prosperity.";
const char* seed2 = "In a world of trust and contribution, Gradido turns gratitude into living value for everyone.";

TEST(EncryptedMemo, EncryptDecryptTest)
{
	auto fullKeyPair1 = KeyPairEd25519::create(Block(seed1));
	auto fullKeyPair2 = KeyPairEd25519::create(Block(seed2));

	EXPECT_STREQ(fullKeyPair1->getPublicKey()->convertToHex().c_str(), "593525666fb4bd619b3565501ac673b3200e84a0aa54d961b5f92237eb1fc3b0");
	EXPECT_STREQ(fullKeyPair2->getPublicKey()->convertToHex().c_str(), "2eb732a70d88fa0bf0e26b157b371dfec77c0d03f9994a733022051b73a8eadb");

	EncryptedMemo memo(
		testMemo,
		AuthenticatedEncryption(*fullKeyPair1),
		AuthenticatedEncryption(*fullKeyPair1)
	);
	auto encryptedMemoHex = memo.getMemo().convertToHex();
	// not possible, because of random nonce result differ on each run
	/*EXPECT_STREQ(
		"732193775a0e51bfddfdcc2f67115b72069522333cc3d857cea0263c5c438c268a3b93ac6498d835a90cd92a75ea8bd8ead19c02d416dbf42b22c22d59fd4ddaabb1ad16c183f1c1e8276a83c3eaf35a89fa7d318f9d0f8f1a271894a3293733a3612dee47d2638951bd8da12490edf2af51243bc0b27be88bff927c520ee633a4decd79ee296ba0",
		encryptedMemoHex.c_str()
	);*/

	auto decryptedMemo = memo.decrypt(AuthenticatedEncryption(*fullKeyPair1), AuthenticatedEncryption(*fullKeyPair1));
	EXPECT_STREQ(testMemo, decryptedMemo.c_str());
}
/*
TEST(EncryptedMemo, EncryptSerializeDeserializeDecryptTest)
{
	auto fullKeyPair1 = KeyPairEd25519::create(Block(seed1));
	auto fullKeyPair2 = KeyPairEd25519::create(Block(seed2));

	EncryptedMemo memo(
		testMemo,
		AuthenticatedEncryption(*fullKeyPair1),
		AuthenticatedEncryption(*fullKeyPair1)
	);

}*/