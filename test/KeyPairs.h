#ifndef __GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H
#define __GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/data/GradidoTransaction.h"

#include <vector>
#include <memory>

struct KeyPair
{
	KeyPair() :
		publicKey(std::make_shared<memory::Block>(32)),
		privateKey(std::make_shared<memory::Block>(64)) {}
	memory::BlockPtr publicKey;
	memory::BlockPtr privateKey;
};

extern std::vector<KeyPair> g_KeyPairs;
void generateKeyPairs();
memory::ConstBlockPtr sign(memory::ConstBlockPtr bodyBytes, const KeyPair& keyPair);

#endif //__GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H