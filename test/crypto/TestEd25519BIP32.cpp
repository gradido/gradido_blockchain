#include "TestEd25519BIP32.h"

#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/crypto/KeyPairEd25519Ex.h"
#include "gradido_blockchain/crypto/keyDerivation.h"
#include "gradido_blockchain/memory/Block.h"
#include "../KeyPairs.h"

#include "ed25519_bip32_c_interface.h"

using namespace std;

void TestEd25519Bip32::SetUp()
{	
	mSeed = std::make_shared<memory::Block>("Understanding different perspectives can lead to innovative solutions and personal growth.");
	mRootKeyPair = KeyPairEd25519::create(*mSeed);
	mChainCode = mRootKeyPair->getChainCode();
	mPublicKey = mRootKeyPair->getPublicKey();
}

void TestEd25519Bip32::TearDown()
{

}

TEST_F(TestEd25519Bip32, TestTestData)
{
	EXPECT_TRUE(mRootKeyPair->isNormalized());
	EXPECT_TRUE(mRootKeyPair->is3rdHighestBitClear());
}

TEST_F(TestEd25519Bip32, TestPrivateDerivationSoft)
{
	int index = 1;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::SOFT);
	auto child1 = mRootKeyPair->deriveChild(index);
	memory::Block derivedPrivateKey(64);
	memory::Block derivedChainCode(32);
	memory::Block derivedPublicKey(32);
	derivePrivateKey(*mSeed, *mChainCode, index, derivedPrivateKey, derivedChainCode);
	getPublicFromExtendedSecret(derivedPrivateKey, derivedChainCode, derivedPublicKey);

	EXPECT_TRUE(derivedPublicKey.isTheSame(child1->getPublicKey()));
	EXPECT_TRUE(derivedChainCode.isTheSame(child1->getChainCode()));
	SecretKeyCryptography secretKey;
	secretKey.createKey("salt", "pwd");
	auto encryptedDerivedPrivateKey = secretKey.encrypt(derivedPrivateKey);
	EXPECT_TRUE(child1->getCryptedPrivKey(secretKey).isTheSame(encryptedDerivedPrivateKey));
}

TEST_F(TestEd25519Bip32, TestPrivateDerivationHard)
{
	int index = 0x80000000;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::HARD);
	auto child1 = mRootKeyPair->deriveChild(index);
	memory::Block derivedPrivateKey(64);
	memory::Block derivedChainCode(32);
	memory::Block derivedPublicKey(32);
	derivePrivateKey(*mSeed, *mChainCode, index, derivedPrivateKey, derivedChainCode);
	getPublicFromExtendedSecret(derivedPrivateKey, derivedChainCode, derivedPublicKey);

	EXPECT_TRUE(derivedPublicKey.isTheSame(child1->getPublicKey()));
	EXPECT_TRUE(derivedChainCode.isTheSame(child1->getChainCode()));
	SecretKeyCryptography secretKey;
	secretKey.createKey("salt", "pwd");
	auto encryptedDerivedPrivateKey = secretKey.encrypt(derivedPrivateKey);
	EXPECT_TRUE(child1->getCryptedPrivKey(secretKey).isTheSame(encryptedDerivedPrivateKey));
}

TEST_F(TestEd25519Bip32, TestPublicDerivationSoft)
{
	int index = 1;
	ASSERT_EQ(KeyPairEd25519::getDerivationType(index), Ed25519DerivationType::SOFT);
	auto publicRootKey = std::make_shared<KeyPairEd25519>(mPublicKey, nullptr, mChainCode);
	auto child1 = publicRootKey->deriveChild(index);
	memory::Block derivedChainCode(32);
	memory::Block derivedPublicKey(32);
	derivePublicKey(*mPublicKey, *mChainCode, index, derivedPublicKey, derivedChainCode);
	memory::Block extendedSecret = memory::Block::fromHex("80c8a93286376d6b70bab4fb8418f51c7217922b31935dbc8abca3bfba056c555168d7f685f7af1151cb215ce623a8e5b94e8d8fc45149dcaaab080b25adac2d");
	memory::Block publicFromRust(32);
	getPublicFromExtendedSecret(extendedSecret.data(), mChainCode->data(), publicFromRust.data());

	EXPECT_TRUE(publicFromRust.isTheSame(mPublicKey));

	EXPECT_TRUE(derivedPublicKey.isTheSame(child1->getPublicKey()));
	EXPECT_TRUE(derivedChainCode.isTheSame(child1->getChainCode()));
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
	memory::Block resultRust(32);
	add_28_mul8(mSeed->data(0), mSeed->data(32), resultRust.data());
	EXPECT_TRUE(resultLocal.isTheSame(resultRust));
}

TEST_F(TestEd25519Bip32, TestFromRustLibrary)
{
	auto privKey = std::make_shared<memory::Block>(std::vector<uint8_t>{
		0xf8, 0xa2, 0x92, 0x31, 0xee, 0x38, 0xd6, 0xc5, 0xbf, 0x71, 0x5d, 0x5b, 0xac, 0x21, 0xc7, 0x50,
		0x57, 0x7a, 0xa3, 0x79, 0x8b, 0x22, 0xd7, 0x9d, 0x65, 0xbf, 0x97, 0xd6, 0xfa, 0xde, 0xa1, 0x5a,
		0xdc, 0xd1, 0xee, 0x1a, 0xbd, 0xf7, 0x8b, 0xd4, 0xbe, 0x64, 0x73, 0x1a, 0x12, 0xde, 0xb9, 0x4d,
		0x36, 0x71, 0x78, 0x41, 0x12, 0xeb, 0x6f, 0x36, 0x4b, 0x87, 0x18, 0x51, 0xfd, 0x1c, 0x9a, 0x24
	});
	auto pubKey = std::make_shared<memory::Block>(memory::Block::fromHex("86ae05cac74c490e097646814c76a9ac813c470afb6ce6429bf836b1bf517878"));
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
	memory::Block childPub(memory::Block::fromHex("9c99845ae0a60881decf7874b94ad02e7540628b588f8b4305d668cc53986978"));
	auto publicKey = std::make_shared<memory::Block>(KeyPairEd25519::calculatePublicKey(*privKey));
	//printf("my calculated pubkey: %s\n", publicKey->convertToHex().data());
	//auto publicKey = std::make_shared<memory::Block>(32);
	//getPublicFromPrivateKey(*privKey, *chainCode, *publicKey);
	//printf("rust calculated pubkey: %s\n", publicKey->convertToHex().data());
	EXPECT_TRUE(publicKey->isTheSame(pubKey));
	KeyPairEd25519 keyPair(publicKey, privKey, chainCode);
	auto child = keyPair.deriveChild(0x80000000);
	EXPECT_TRUE(child->getChainCode()->isTheSame(childChainCode));
	EXPECT_TRUE(child->getPublicKey()->isTheSame(childPub));
	SecretKeyCryptography secretKey;
	secretKey.createKey("salt", "pwd");
	auto encryptedDerivedPrivateKey = secretKey.encrypt(childPriv);
	EXPECT_TRUE(child->getCryptedPrivKey(secretKey).isTheSame(encryptedDerivedPrivateKey));
}

TEST_F(TestEd25519Bip32, calculatePublicKey)
{
	auto keyPair = KeyPairEd25519::create(*mSeed);
	memory::Block rustPublic(32);
	getPublicFromSeed(*mSeed, *keyPair->getChainCode(), rustPublic);
	EXPECT_TRUE(keyPair->getPublicKey()->isTheSame(rustPublic));
}

TEST_F(TestEd25519Bip32, signVerifyTest)
{
	memory::Block message(
		"Let the power of knowledge and innovation guide humanity towards a future where energy is infinite, borders are meaningless, and every mind is free to dream.Together, we can unlock the potential of this world and beyond. - Inspired by the vision of Nikola Tesla"
	);
	auto signature = mRootKeyPair->sign(message);
	//printf("signature: %s\n", signature.convertToHex().data());
	EXPECT_EQ(signature.convertToHex(), "c6486839dff5f568c5757cd22556a0df545c4c738ca87660b96721d0a8f7f897b7da062a7c1ba9c1209071a3282b882a10cf31c5b78ea9441f8cf606780a9a0d");
	EXPECT_TRUE(mRootKeyPair->verify(message, signature));
}

TEST_F(TestEd25519Bip32, publicPrivateKeyDerivation)
{
	uint32_t index = 10;
	auto child1 = mRootKeyPair->deriveChild(index);
	KeyPairEd25519 publicParent(mPublicKey, nullptr, mChainCode);
	auto child2 = publicParent.deriveChild(index);
	EXPECT_EQ(child1->getPublicKey()->convertToHex(), child2->getPublicKey()->convertToHex());
}

TEST_F(TestEd25519Bip32, signVerifyWithChild)
{
	uint32_t index = 10;
	auto child = mRootKeyPair->deriveChild(index);
	memory::Block message(
		"Let the power of knowledge and innovation guide humanity towards a future where energy is infinite, borders are meaningless, and every mind is free to dream.Together, we can unlock the potential of this world and beyond. - Inspired by the vision of Nikola Tesla"
	);
	auto signature = child->sign(message);
	//printf("signature: %s\n", signature.convertToHex().data());
	EXPECT_EQ(signature.convertToHex(), "bbb451ae7a0fda8fe7cafef02de27245e017579505c5df33fbc8a3191c8030eec4207f9cee44be513c67c03fe0866e38fd29f6fbef56f0304a5b96ec77db0e02");
	EXPECT_TRUE(child->verify(message, signature));
}