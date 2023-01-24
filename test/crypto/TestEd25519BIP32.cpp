#include "TestEd25519BIP32.h"

#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

#include "ed25519_bip32_c_interface.h"

void TestEd25519Bip32::SetUp()
{
	std::unique_ptr<KeyPairEd25519> key_pair;
	bool valid_key_pair = false;
	int run_again = 0;
	int max_while_count = 0;
	Profiler timeUsed;
	CryptoConfig::loadMnemonicWordLists();
	for (int i = 0; i < 100; i++) {
		int while_count = 0;
		do {
			if (while_count == 1) {
				run_again++;
			}
			auto passphrase = Passphrase::generate(&CryptoConfig::g_Mnemonic_WordLists[2]);
			key_pair = KeyPairEd25519::create(passphrase);
			valid_key_pair = is_3rd_highest_bit_clear(key_pair->getPrivateKey()->data(), key_pair->getChainCode()->data());
			while_count++;
		} while (!valid_key_pair);
		if (while_count > max_while_count) {
			max_while_count = while_count;
		}
		mED25519KeyPairs.push_back(std::move(key_pair));
		//auto privKey = DataTypeConverter::hexToBin(valid_private_keys[i]);
		//mED25519KeyPairs.push_back(new KeyPairEd25519(privKey));
	}
	printf("run again: %d, max while count: %d, time: %s\n", run_again, max_while_count, timeUsed.string().data());
}

void TestEd25519Bip32::TearDown()
{
	mED25519KeyPairs.clear();
}

TEST_F(TestEd25519Bip32, TestPrivateToPublic)
{
	auto mm = MemoryManager::getInstance();
	auto public_key_temp = mm->getMemory(32);
	memset(*public_key_temp, 0, 32);
	Profiler timeUsed;
	for (auto it = mED25519KeyPairs.begin(); it != mED25519KeyPairs.end(); it++) {
		getPublicFromPrivateKey(*(*it)->getPrivateKey(), *(*it)->getChainCode(), *public_key_temp);
		ASSERT_TRUE((*it)->isTheSame(*public_key_temp));
	}
	printf("time for %d calls to getPublicFromPrivateKey in rust: %s\n", (uint32_t)mED25519KeyPairs.size(), timeUsed.string().data());
	mm->releaseMemory(public_key_temp);
}

TEST_F(TestEd25519Bip32, TestDerivePublic)
{
	auto mm = MemoryManager::getInstance();
	auto derived_public_temp = mm->getMemory(32);
	auto derived_chain_code_temp = mm->getMemory(32);
	auto normalized_public = mm->getMemory(32);
	auto extended = mm->getMemory(64);

	Profiler timeUsed;
	int succeed = 0;
	for (auto it = mED25519KeyPairs.begin(); it != mED25519KeyPairs.end(); it++) {
		auto index = rand();
		getPrivateExtended((*it)->getPrivateKey()->data(), (*it)->getChainCode()->data(),
			*extended, *normalized_public);
		auto result = derivePublicKey(
			(*it)->getPublicKey(),
			(*it)->getChainCode()->data(),
			index,
			derived_public_temp->data(),
			derived_chain_code_temp->data()
		);
		if (!result) {
			//printf("failed with index: %d\n", index);
			printf("normalized: %s\n", DataTypeConverter::binToHex(normalized_public).data());
			printf("original  : %s\n", (*it)->getPublicKeyHex().data());
		}
		else {
			//printf("success with index: %d\n", index);
			if (!succeed) {
				printf("extend: %s\n", DataTypeConverter::binToHex(extended).data());
				printf("chain : %s\n", (*it)->getChainCodeHex().data());
				printf("public: %s\n", (*it)->getPublicKeyHex().data());
				printf("seed  : %s\n", DataTypeConverter::binToHex((*it)->getPrivateKey()).data());
			}
			succeed++;
		}
		//ASSERT_TRUE(result);
		EXPECT_FALSE((*it)->isTheSame(*derived_public_temp));
		EXPECT_NE(memcmp((*it)->getChainCode()->data(), derived_chain_code_temp->data(), 32), 0);
	} 
	printf("time for %d calls to derivePublicKey in rust: %s, succeed: %d\n", (uint32_t)mED25519KeyPairs.size(), timeUsed.string().data(), succeed);

	mm->releaseMemory(derived_public_temp);
	mm->releaseMemory(derived_chain_code_temp);
	mm->releaseMemory(extended);
	mm->releaseMemory(normalized_public);
}