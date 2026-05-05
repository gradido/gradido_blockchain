#include "TestEd25519BIP32.h"

#include "gradido_blockchain/lib/MonotonicTimer.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/crypto/KeyPairEd25519Ex.h"
#include "gradido_blockchain/crypto/keyDerivation.h"
#include "gradido_blockchain/memory/Block.h"
#include "../KeyPairs.h"

using namespace std;

void TestEd25519Bip32::SetUp()
{
	mSeed = std::make_shared<memory::Block>("Understanding different perspectives can lead to innovative solutions and personal growth.");
	mRootKeyPair = TestKeyPairEd25519::create(*mSeed);
	ASSERT_TRUE(mRootKeyPair);
	mChainCode = mRootKeyPair->getChainCode();
	mPublicKey = mRootKeyPair->getPublicKey();
}

void TestEd25519Bip32::TearDown()
{

}
/*
TEST_F(TestEd25519Bip32, TestTestData)
{
	EXPECT_TRUE(mRootKeyPair->isNormalized());
	EXPECT_TRUE(mRootKeyPair->is3rdHighestBitClear());
}
*/
TEST_F(TestEd25519Bip32, TestPrivateDerivationSoft)
{
	int index = 1;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::SOFT);
	EXPECT_THROW(mRootKeyPair->deriveChild(index), Ed25519DeriveException);
	/*auto child = mRootKeyPair->deriveChild(index);
	EXPECT_EQ(child->getPublicKey()->convertToHex(), "08074fb5fd5c630f0e56a08e1e2bd93a2d10e6edaeb56ca72ca1deb8d8363630");
	EXPECT_EQ(child->getChainCode()->convertToHex(), "72f1fbf070eed9acbd40f3ba3998d985c734747afd4ffcd22824ceba9249ad71");
	EXPECT_EQ(child->getPrivateKey()->convertToHex(), "e082429a76d41bb4698dc80102c816213493347c47c41023755e0a8cc0056c551943624dd91fe356a907e782bdd76fe3c3be105111987a6583b39df8bbaf7f05");
	*/
}

TEST_F(TestEd25519Bip32, TestPrivateDerivationHard)
{
	int index = 0x80000000;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::HARD);
	auto child = mRootKeyPair->deriveChild(index);
	EXPECT_EQ(child->getPublicKey()->convertToHex(), "20d006e3205f15230f17f6e450bb0f68afb9b17d40b12b5dd6ca1a5552007da1");
	EXPECT_EQ(child->getChainCode()->convertToHex(), "75afe17648978f47888db56c61ade7e258a5005abb21d21f2408b81146388cd2");
	EXPECT_EQ(child->getPrivateKey()->convertToHex(), "68cbeddaacd1ee5966558a08c82ddccbd6b257065d2be069c7354af3dfceb08620d006e3205f15230f17f6e450bb0f68afb9b17d40b12b5dd6ca1a5552007da1");
}

TEST_F(TestEd25519Bip32, TestPublicDerivationSoft)
{
	int index = 1;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::SOFT);
	auto publicRootKey = std::make_shared<TestKeyPairEd25519>(mPublicKey, nullptr, mChainCode);
	EXPECT_THROW(publicRootKey->deriveChild(index), Ed25519DeriveException);
	/*EXPECT_EQ(child->getPublicKey()->convertToHex(), "08074fb5fd5c630f0e56a08e1e2bd93a2d10e6edaeb56ca72ca1deb8d8363630");
	EXPECT_EQ(child->getChainCode()->convertToHex(), "72f1fbf070eed9acbd40f3ba3998d985c734747afd4ffcd22824ceba9249ad71");
	EXPECT_FALSE(child->getPrivateKey());*/
}

TEST_F(TestEd25519Bip32, TestPublicDerivationHard)
{
	int index = 0x80000000;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::HARD);
	auto publicRootKey = std::make_shared<KeyPairEd25519>(mPublicKey, nullptr, mChainCode);
	EXPECT_THROW(publicRootKey->deriveChild(index), Ed25519DeriveException);
}

TEST_F(TestEd25519Bip32, TestLowLevelDerivationHelper)
{
	memory::Block resultLocal(32);
	auto resultSpan = std::span<uint8_t, 32>{ resultLocal.data(0), 32 };
	auto kl = std::span<const uint8_t, 32>{ mSeed->data(0), 32 };
	auto kr = std::span<const uint8_t, 32>{ mSeed->data(32), 32 };

	keyDerivation::add28Mul8(resultSpan, kl, kr);
	EXPECT_EQ(resultLocal.convertToHex(), "9d219000768c7fd471c594798a2305e569afd8e5e02180c3bb189e7671656374");
}
/*
// ed25519-bip32
TEST_F(TestEd25519Bip32, TestFromRustLibrary)
{
	auto privKey = std::make_shared<memory::Block>(std::vector<uint8_t>{
		0xf8, 0xa2, 0x92, 0x31, 0xee, 0x38, 0xd6, 0xc5, 0xbf, 0x71, 0x5d, 0x5b, 0xac, 0x21, 0xc7, 0x50,
		0x57, 0x7a, 0xa3, 0x79, 0x8b, 0x22, 0xd7, 0x9d, 0x65, 0xbf, 0x97, 0xd6, 0xfa, 0xde, 0xa1, 0x5a,
		0xdc, 0xd1, 0xee, 0x1a, 0xbd, 0xf7, 0x8b, 0xd4, 0xbe, 0x64, 0x73, 0x1a, 0x12, 0xde, 0xb9, 0x4d,
		0x36, 0x71, 0x78, 0x41, 0x12, 0xeb, 0x6f, 0x36, 0x4b, 0x87, 0x18, 0x51, 0xfd, 0x1c, 0x9a, 0x24
	});
	auto pubKey = std::make_shared<memory::Block>(memory::Block::fromHex("86ae05cac74c490e097646814c76a9ac813c470afb6ce6429bf836b1bf517878", 64));
	auto chainCode = std::make_shared<memory::Block>(std::vector<uint8_t>{
		0x73, 0x84, 0xdb, 0x9a, 0xd6, 0x00, 0x3b, 0xbd, 0x08, 0xb3, 0xb1, 0xdd, 0xc0, 0xd0, 0x7a, 0x59,
		0x72, 0x93, 0xff, 0x85, 0xe9, 0x61, 0xbf, 0x25, 0x2b, 0x33, 0x12, 0x62, 0xed, 0xdf, 0xad, 0x0d
	});
	memory::Block childPriv(std::vector<uint8_t>{
		0x60, 0xd3, 0x99, 0xda, 0x83, 0xef, 0x80, 0xd8, 0xd4, 0xf8, 0xd2, 0x23, 0x23, 0x9e, 0xfd, 0xc2,
		0xb8, 0xfe, 0xf3, 0x87, 0xe1, 0xb5, 0x21, 0x91, 0x37, 0xff, 0xb4, 0xe8, 0xfb, 0xde, 0xa1, 0x5a,
		0xdc, 0x93, 0x66, 0xb7, 0xd0, 0x03, 0xaf, 0x37, 0xc1, 0x13, 0x96, 0xde, 0x9a, 0x83, 0x73, 0x4e,
		0x30, 0xe0, 0x5e, 0x85, 0x1e, 0xfa, 0x32, 0x74, 0x5c, 0x9c, 0xd7, 0xb4, 0x27, 0x12, 0xc8, 0x90
	});
	memory::Block childChainCode(std::vector<uint8_t>{
		0x60, 0x87, 0x63, 0x77, 0x0e, 0xdd, 0xf7, 0x72, 0x48, 0xab, 0x65, 0x29, 0x84, 0xb2, 0x1b, 0x84,
		0x97, 0x60, 0xd1, 0xda, 0x74, 0xa6, 0xf5, 0xbd, 0x63, 0x3c, 0xe4, 0x1a, 0xdc, 0xee, 0xf0, 0x7a
	});
	memory::Block childPub(memory::Block::fromHex("9c99845ae0a60881decf7874b94ad02e7540628b588f8b4305d668cc53986978", 64));
	auto publicKey = std::make_shared<memory::Block>(KeyPairEd25519::calculatePublicKey(*privKey));
	//printf("my calculated pubkey: %s\n", publicKey->convertToHex().data());
	//auto publicKey = std::make_shared<memory::Block>(32);
	//getPublicFromPrivateKey(*privKey, *chainCode, *publicKey);
	//printf("rust calculated pubkey: %s\n", publicKey->convertToHex().data());
	EXPECT_TRUE(publicKey->isTheSame(pubKey));
	TestKeyPairEd25519 keyPair(publicKey, privKey, chainCode);
	auto child = keyPair.deriveChild(0x80000000);

	EXPECT_EQ(child->getPublicKey()->convertToHex(), "9c99845ae0a60881decf7874b94ad02e7540628b588f8b4305d668cc53986978");
	EXPECT_EQ(child->getChainCode()->convertToHex(), "608763770eddf77248ab652984b21b849760d1da74a6f5bd633ce41adceef07a");
	EXPECT_EQ(child->getPrivateKey()->convertToHex(), "60d399da83ef80d8d4f8d223239efdc2b8fef387e1b5219137ffb4e8fbdea15adc9366b7d003af37c11396de9a83734e30e05e851efa32745c9cd7b42712c890");
}
*/
TEST_F(TestEd25519Bip32, signVerifyTest)
{
	memory::Block message(
		"Let the power of knowledge and innovation guide humanity towards a future where energy is infinite, borders are meaningless, and every mind is free to dream.Together, we can unlock the potential of this world and beyond. - Inspired by the vision of Nikola Tesla"
	);
	auto signature = mRootKeyPair->sign(message);
	//printf("signature: %s\n", signature.convertToHex().data());
	EXPECT_EQ(signature.convertToHex(), "083873fb3426feb21413ac397a73155257a10dd8df729cdcf42f3f3dd06df5d70b32f37541cc70fc036320ab72b3eb5c5a1042593ee4cad849c1d90063bbd301");
	EXPECT_TRUE(mRootKeyPair->verify(message, signature));
}
/*
TEST_F(TestEd25519Bip32, publicPrivateKeyDerivation)
{
	uint32_t index = 10;
	auto child1 = mRootKeyPair->deriveChild(index);
	KeyPairEd25519 publicParent(mPublicKey, nullptr, mChainCode);
	auto child2 = publicParent.deriveChild(index);
	EXPECT_EQ(child1->getPublicKey()->convertToHex(), child2->getPublicKey()->convertToHex());
}*/

TEST_F(TestEd25519Bip32, signVerifyWithChild)
{
	uint32_t index = 10 | 0x80000000;
	auto child = mRootKeyPair->deriveChild(index);
	memory::Block message(
		"Let the power of knowledge and innovation guide humanity towards a future where energy is infinite, borders are meaningless, and every mind is free to dream.Together, we can unlock the potential of this world and beyond. - Inspired by the vision of Nikola Tesla"
	);
	auto signature = child->sign(message);
	//printf("signature: %s\n", signature.convertToHex().data());
	EXPECT_EQ(signature.convertToHex(), "5ed404efafdbc14a4f6ad5d2b886d63766f2a7784262e1b79ea16cfbea6b351c081db82305e11725354085a7fd8eacb28cda5a642ea8755391234837a71bf001");
	EXPECT_TRUE(child->verify(message, signature));
}