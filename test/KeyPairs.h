#ifndef __GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H
#define __GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H

#include "gradido_blockchain/MemoryManager.h"

#include <vector>
#include <memory>

struct KeyPair
{
	KeyPair() :
		publicKey(nullptr),
		privateKey(nullptr) {}
	~KeyPair() {
		auto mm = MemoryManager::getInstance();
		if (publicKey) {
			mm->releaseMemory(publicKey);
			mm->releaseMemory(privateKey);
		}
	}
	MemoryBin* publicKey;
	MemoryBin* privateKey;
};

extern std::vector<std::shared_ptr<KeyPair>> g_KeyPairs;
void generateKeyPairs();

#endif //__GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H