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

	EXPECT_STREQ(fullKeyPair1->getPublicKey()->convertToHex().c_str(), "dba53616f61204300f11e35e09864b7368256edfe6f7d40ee03041d0c6b1164a");
	EXPECT_STREQ(fullKeyPair2->getPublicKey()->convertToHex().c_str(), "9eb2cad43e7ccfaeb2de6befffa2cd5cec3c9dfad87a3fd4389c748242b920eb");

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