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

TEST_F(TestEd25519Bip32, signVerifyNegativeTest)
{
	memory::Block message(
		"Let the power of knowledge and innovation guide humanity towards a future where energy is infinite, borders are meaningless, and every mind is free to dream.Together, we can unlock the potential of this world and beyond. - Inspired by the vision of Nikola Tesla"
	);
	memory::Block signature(64);
	//printf("signature: %s\n", signature.convertToHex().data());
	EXPECT_FALSE(mRootKeyPair->verify(message, signature));
	randombytes_buf(signature.data(), signature.size());	
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
// https://slips.readthedocs.io/en/latest/slip-0010/#test-vectors
TEST_F(TestEd25519Bip32, SLIP0010TestVectors1)
{
	std::string hexSeed("000102030405060708090a0b0c0d0e0f");	
	std::string testPayload = "Test Payload for sign";
	auto root = TestKeyPairEd25519::create(MemoryBin::fromHex(hexSeed));

	// test root
	EXPECT_EQ(root->getSlip10PublicKey().convertToHex(), "00a4b2856bfec510abab89753fac1ac0e1112364e7d250545963f135f2a33188ed");
	EXPECT_EQ(root->getChainCode()->convertToHex(), "90046a93de5380a72b5e45010748567d5ea02bbf6522f979e05c0d8d8ca9fffb");
	EXPECT_EQ(root->getPrivateKey()->convertToHex().substr(0, 64), "2b4be7f19ee27bbf30c667b642d5f4aa69fd169872f8fc3059c08ebae2eb19e7");
	EXPECT_EQ(root->getPrivateKey()->convertToHex().substr(64), root->getPublicKey()->convertToHex());

	auto signature = root->sign(testPayload);
	EXPECT_TRUE(root->verify(testPayload, signature.copyAsString()));

	// Chain m/0H
	auto c0 = root->deriveChild(0x80000000);
	EXPECT_EQ(c0->getSlip10PublicKey().convertToHex(), "008c8a13df77a28f3445213a0f432fde644acaa215fc72dcdf300d5efaa85d350c");
	EXPECT_EQ(c0->getChainCode()->convertToHex(), "8b59aa11380b624e81507a27fedda59fea6d0b779a778918a2fd3590e16e9c69");
	EXPECT_EQ(c0->getPrivateKey()->convertToHex().substr(0, 64), "68e0fe46dfb67e368c75379acec591dad19df3cde26e63b93a8e704f1dade7a3");
	EXPECT_EQ(c0->getPrivateKey()->convertToHex().substr(64), c0->getPublicKey()->convertToHex());

	signature = c0->sign(testPayload);
	EXPECT_TRUE(c0->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/1
	auto c01 = c0->deriveChild(0x80000001);
	EXPECT_EQ(c01->getSlip10PublicKey().convertToHex(), "001932a5270f335bed617d5b935c80aedb1a35bd9fc1e31acafd5372c30f5c1187");
	EXPECT_EQ(c01->getChainCode()->convertToHex(), "a320425f77d1b5c2505a6b1b27382b37368ee640e3557c315416801243552f14");
	EXPECT_EQ(c01->getPrivateKey()->convertToHex().substr(0, 64), "b1d0bad404bf35da785a64ca1ac54b2617211d2777696fbffaf208f746ae84f2");
	EXPECT_EQ(c01->getPrivateKey()->convertToHex().substr(64), c01->getPublicKey()->convertToHex());

	signature = c01->sign(testPayload);
	EXPECT_TRUE(c01->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/1/2H
	auto c012 = c01->deriveChild(0x80000002);
	EXPECT_EQ(c012->getSlip10PublicKey().convertToHex(), "00ae98736566d30ed0e9d2f4486a64bc95740d89c7db33f52121f8ea8f76ff0fc1");
	EXPECT_EQ(c012->getChainCode()->convertToHex(), "2e69929e00b5ab250f49c3fb1c12f252de4fed2c1db88387094a0f8c4c9ccd6c");
	EXPECT_EQ(c012->getPrivateKey()->convertToHex().substr(0, 64), "92a5b23c0b8a99e37d07df3fb9966917f5d06e02ddbd909c7e184371463e9fc9");
	EXPECT_EQ(c012->getPrivateKey()->convertToHex().substr(64), c012->getPublicKey()->convertToHex());

	signature = c012->sign(testPayload);
	EXPECT_TRUE(c012->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/1/2H/2
	auto c0122 = c012->deriveChild(0x80000002);
	EXPECT_EQ(c0122->getSlip10PublicKey().convertToHex(), "008abae2d66361c879b900d204ad2cc4984fa2aa344dd7ddc46007329ac76c429c");
	EXPECT_EQ(c0122->getChainCode()->convertToHex(), "8f6d87f93d750e0efccda017d662a1b31a266e4a6f5993b15f5c1f07f74dd5cc");
	EXPECT_EQ(c0122->getPrivateKey()->convertToHex().substr(0, 64), "30d1dc7e5fc04c31219ab25a27ae00b50f6fd66622f6e9c913253d6511d1e662");
	EXPECT_EQ(c0122->getPrivateKey()->convertToHex().substr(64), c0122->getPublicKey()->convertToHex());

	signature = c0122->sign(testPayload);
	EXPECT_TRUE(c0122->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/1/2H/2/1000000000
	auto c01221Mrd = c0122->deriveChild(0x80000000 + 1000000000);
	EXPECT_EQ(c01221Mrd->getSlip10PublicKey().convertToHex(), "003c24da049451555d51a7014a37337aa4e12d41e485abccfa46b47dfb2af54b7a");
	EXPECT_EQ(c01221Mrd->getChainCode()->convertToHex(), "68789923a0cac2cd5a29172a475fe9e0fb14cd6adb5ad98a3fa70333e7afa230");
	EXPECT_EQ(c01221Mrd->getPrivateKey()->convertToHex().substr(0, 64), "8f94d394a8e8fd6b1bc2f3f49f5c47e385281d5c17e65324b0f62483e37e8793");
	EXPECT_EQ(c01221Mrd->getPrivateKey()->convertToHex().substr(64), c01221Mrd->getPublicKey()->convertToHex());

	signature = c01221Mrd->sign(testPayload);
	EXPECT_TRUE(c01221Mrd->verify(testPayload, signature.copyAsString()));
}
TEST_F(TestEd25519Bip32, SLIP0010TestVectors2)
{
	std::string hexSeed("fffcf9f6f3f0edeae7e4e1dedbd8d5d2cfccc9c6c3c0bdbab7b4b1aeaba8a5a29f9c999693908d8a8784817e7b7875726f6c696663605d5a5754514e4b484542");
	std::string testPayload = "Test Payload for sign2";
	auto root = TestKeyPairEd25519::create(MemoryBin::fromHex(hexSeed));

	// test root
	EXPECT_EQ(root->getSlip10PublicKey().convertToHex(), "008fe9693f8fa62a4305a140b9764c5ee01e455963744fe18204b4fb948249308a");
	EXPECT_EQ(root->getChainCode()->convertToHex(), "ef70a74db9c3a5af931b5fe73ed8e1a53464133654fd55e7a66f8570b8e33c3b");
	EXPECT_EQ(root->getPrivateKey()->convertToHex().substr(0, 64), "171cb88b1b3c1db25add599712e36245d75bc65a1a5c9e18d76f9f2b1eab4012");
	EXPECT_EQ(root->getPrivateKey()->convertToHex().substr(64), root->getPublicKey()->convertToHex());

	auto signature = root->sign(testPayload);
	EXPECT_TRUE(root->verify(testPayload, signature.copyAsString()));

	// Chain m/0H
	auto c0 = root->deriveChild(0x80000000);
	EXPECT_EQ(c0->getSlip10PublicKey().convertToHex(), "0086fab68dcb57aa196c77c5f264f215a112c22a912c10d123b0d03c3c28ef1037");
	EXPECT_EQ(c0->getChainCode()->convertToHex(), "0b78a3226f915c082bf118f83618a618ab6dec793752624cbeb622acb562862d");
	EXPECT_EQ(c0->getPrivateKey()->convertToHex().substr(0, 64), "1559eb2bbec5790b0c65d8693e4d0875b1747f4970ae8b650486ed7470845635");
	EXPECT_EQ(c0->getPrivateKey()->convertToHex().substr(64), c0->getPublicKey()->convertToHex());

	signature = c0->sign(testPayload);
	EXPECT_TRUE(c0->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/2147483647H
	auto c0_2147483647 = c0->deriveChild(0x80000000 + 2147483647);
	EXPECT_EQ(c0_2147483647->getSlip10PublicKey().convertToHex(), "005ba3b9ac6e90e83effcd25ac4e58a1365a9e35a3d3ae5eb07b9e4d90bcf7506d");
	EXPECT_EQ(c0_2147483647->getChainCode()->convertToHex(), "138f0b2551bcafeca6ff2aa88ba8ed0ed8de070841f0c4ef0165df8181eaad7f");
	EXPECT_EQ(c0_2147483647->getPrivateKey()->convertToHex().substr(0, 64), "ea4f5bfe8694d8bb74b7b59404632fd5968b774ed545e810de9c32a4fb4192f4");
	EXPECT_EQ(c0_2147483647->getPrivateKey()->convertToHex().substr(64), c0_2147483647->getPublicKey()->convertToHex());

	signature = c0_2147483647->sign(testPayload);
	EXPECT_TRUE(c0_2147483647->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/2147483647H/1H
	auto c0_2147483647_1 = c0_2147483647->deriveChild(0x80000001);
	EXPECT_EQ(c0_2147483647_1->getSlip10PublicKey().convertToHex(), "002e66aa57069c86cc18249aecf5cb5a9cebbfd6fadeab056254763874a9352b45");
	EXPECT_EQ(c0_2147483647_1->getChainCode()->convertToHex(), "73bd9fff1cfbde33a1b846c27085f711c0fe2d66fd32e139d3ebc28e5a4a6b90");
	EXPECT_EQ(c0_2147483647_1->getPrivateKey()->convertToHex().substr(0, 64), "3757c7577170179c7868353ada796c839135b3d30554bbb74a4b1e4a5a58505c");
	EXPECT_EQ(c0_2147483647_1->getPrivateKey()->convertToHex().substr(64), c0_2147483647_1->getPublicKey()->convertToHex());

	signature = c0_2147483647_1->sign(testPayload);
	EXPECT_TRUE(c0_2147483647_1->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/2147483647H/1H/2147483646H
	auto c0_2147483647_1_2147483646 = c0_2147483647_1->deriveChild(0x80000000 + 2147483646);
	EXPECT_EQ(c0_2147483647_1_2147483646->getSlip10PublicKey().convertToHex(), "00e33c0f7d81d843c572275f287498e8d408654fdf0d1e065b84e2e6f157aab09b");
	EXPECT_EQ(c0_2147483647_1_2147483646->getChainCode()->convertToHex(), "0902fe8a29f9140480a00ef244bd183e8a13288e4412d8389d140aac1794825a");
	EXPECT_EQ(c0_2147483647_1_2147483646->getPrivateKey()->convertToHex().substr(0, 64), "5837736c89570de861ebc173b1086da4f505d4adb387c6a1b1342d5e4ac9ec72");
	EXPECT_EQ(c0_2147483647_1_2147483646->getPrivateKey()->convertToHex().substr(64), c0_2147483647_1_2147483646->getPublicKey()->convertToHex());

	signature = c0_2147483647_1_2147483646->sign(testPayload);
	EXPECT_TRUE(c0_2147483647_1_2147483646->verify(testPayload, signature.copyAsString()));

	// Chain m/0H/2147483647H/1H/2147483646H/2H
	auto c0_2147483647_1_2147483646_2 = c0_2147483647_1_2147483646->deriveChild(0x80000002);
	EXPECT_EQ(c0_2147483647_1_2147483646_2->getSlip10PublicKey().convertToHex(), "0047150c75db263559a70d5778bf36abbab30fb061ad69f69ece61a72b0cfa4fc0");
	EXPECT_EQ(c0_2147483647_1_2147483646_2->getChainCode()->convertToHex(), "5d70af781f3a37b829f0d060924d5e960bdc02e85423494afc0b1a41bbe196d4");
	EXPECT_EQ(c0_2147483647_1_2147483646_2->getPrivateKey()->convertToHex().substr(0, 64), "551d333177df541ad876a60ea71f00447931c0a9da16f227c11ea080d7391b8d");
	EXPECT_EQ(c0_2147483647_1_2147483646_2->getPrivateKey()->convertToHex().substr(64), c0_2147483647_1_2147483646_2->getPublicKey()->convertToHex());

	signature = c0_2147483647_1_2147483646_2->sign(testPayload);
	EXPECT_TRUE(c0_2147483647_1_2147483646_2->verify(testPayload, signature.copyAsString()));
}

TEST_F(TestEd25519Bip32, RetryTest)
{
	std::string hexSeed("000102030405060708090a0b0c0d0e0f");
	auto root = TestKeyPairEd25519::create(MemoryBin::fromHex(hexSeed));
	auto a = root->deriveChild(0x80000000);
	auto b = root->deriveChild(0x80000000);

	EXPECT_TRUE(a->isTheSame(b));
}