#include "TestEd25519BIP32.h"

#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/memory/Block.h"

#include "ed25519_bip32_c_interface.h"

using namespace std;

void TestEd25519Bip32::SetUp()
{
	KeyPair keyPair;
	unsigned char hash[crypto_hash_sha512_BYTES];
	std::string seed("testSeed");
	crypto_hash_sha512(hash, (const unsigned char*)seed.data(), seed.size());
	crypto_sign_seed_keypair(keyPair.publicKey, keyPair.privateKey, hash);

	memcpy(keyPair.chainCode, &hash[32], 32);


	bool valid_key_pair = false;
	int run_again = 0;
	int max_while_count = 0;
	Profiler timeUsed;

	for (int i = 0; i < 100; i++) {
		int while_count = 0;
		do {
			if (while_count == 1) {
				run_again++;
			}
			valid_key_pair = is_3rd_highest_bit_clear(keyPair.privateKey, keyPair.chainCode);
			while_count++;
		} while (!valid_key_pair);
		if (while_count > max_while_count) {
			max_while_count = while_count;
		}
		mED25519KeyPairs.push_back(keyPair);
	}
	//printf("run again: %d, max while count: %d, time: %s\n", run_again, max_while_count, timeUsed.string().data());
}

void TestEd25519Bip32::TearDown()
{
	mED25519KeyPairs.clear();
}

TEST_F(TestEd25519Bip32, TestPrivateToPublic)
{	
	memory::Block public_key_temp(32);
	memset(public_key_temp, 0, 32);
	Profiler timeUsed;
	for (auto it = mED25519KeyPairs.begin(); it != mED25519KeyPairs.end(); it++) {
		getPublicFromPrivateKey(it->privateKey, it->chainCode, public_key_temp);
		ASSERT_TRUE(it->publicKey == public_key_temp);
	}
	printf("time for %d calls to getPublicFromPrivateKey in rust: %s\n", (uint32_t)mED25519KeyPairs.size(), timeUsed.string().data());
}

TEST_F(TestEd25519Bip32, TestDerivePublic)
{
	memory::Block derived_public_temp(32);
	memory::Block derived_chain_code_temp(32);
	memory::Block normalized_public(32);
	memory::Block extended(64);

	Profiler timeUsed;
	int succeed = 0;
	for (auto it = mED25519KeyPairs.begin(); it != mED25519KeyPairs.end(); it++) {
		auto index = rand();
		getPrivateExtended(it->privateKey, it->chainCode, extended, normalized_public);
		auto result = derivePublicKey(
			it->publicKey,
			it->chainCode,
			index,
			derived_public_temp,
			derived_chain_code_temp
		);
		if (!result) {
			//printf("failed with index: %d\n", index);
			//printf("normalized: %s\n", normalized_public.convertToHex().data());
			//printf("original  : %s\n", it->publicKey.convertToHex().data());
		}
		else {
			//printf("success with index: %d\n", index);
			if (!succeed) {
				//printf("extend: %s\n", extended.convertToHex().data());
				//printf("chain : %s\n", it->chainCode.convertToHex().data());
				//printf("public: %s\n", it->publicKey.convertToHex().data());
				//printf("seed  : %s\n", it->privateKey.convertToHex().data());
			}
			succeed++;
		}
		//ASSERT_TRUE(result);
		EXPECT_FALSE(it->publicKey == derived_public_temp);
		EXPECT_NE(memcmp(it->chainCode, derived_chain_code_temp, 32), 0);
	} 
	printf("time for %d calls to derivePublicKey in rust: %s, succeed: %d\n", (uint32_t)mED25519KeyPairs.size(), timeUsed.string().data(), succeed);	
}