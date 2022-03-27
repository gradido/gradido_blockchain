#include "gradido_blockchain/crypto/SealedBoxes.h"

namespace SealedBoxes {
	MemoryBin* encrypt(KeyPairEd25519* keys, const std::string& message)
	{
		assert(keys);
		auto mm = MemoryManager::getInstance();
		//int crypto_box_seal(unsigned char* c, const unsigned char* m, unsigned long long mlen, const unsigned char* pk);
		auto encryptedMessage = mm->getMemory(crypto_box_SEALBYTES + message.size());
		crypto_box_seal(*encryptedMessage, (const unsigned char*)message.data(), message.size(), keys->getPublicKey());
		return encryptedMessage;
	}

	std::string decrypt(KeyPairEd25519* keys, const MemoryBin* encryptedMessage)
	{
		assert(keys && keys->hasPrivateKey() && encryptedMessage);
		// int crypto_box_seal_open(unsigned char* m, const unsigned char* c,
		//     unsigned long long clen,
		// 	   const unsigned char* pk, const unsigned char* sk);
		auto mm = MemoryManager::getInstance();
		MemoryBin* temp = mm->getMemory(encryptedMessage->size() - crypto_box_SEALBYTES);
		crypto_box_seal_open(*temp, *encryptedMessage, encryptedMessage->size(),
			keys->getPublicKey(), *keys->getPrivateKey());

		std::string clearMessage((const char*)temp->data(), temp->size());
		mm->releaseMemory(temp);
		return std::move(clearMessage);
	}
}
