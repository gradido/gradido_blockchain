#ifndef __GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H
#define __GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/data/GradidoTransaction.h"

#include <vector>
#include <memory>

extern std::vector<std::shared_ptr<KeyPairEd25519>> g_KeyPairs;
void generateKeyPairs();

#endif //__GRADIDO_BLOCKCHAIN_TEST_KEY_PAIRS_H