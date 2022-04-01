#include "gradido_blockchain/crypto/SealedBoxes.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

namespace SealedBoxes {
	MemoryBin* encrypt(const AuthenticatedEncryption* keys, const std::string& message)
	{
		assert(keys);
		auto mm = MemoryManager::getInstance();
		//int crypto_box_seal(unsigned char* c, const unsigned char* m, unsigned long long mlen, const unsigned char* pk);
		auto encryptedMessage = mm->getMemory(crypto_box_SEALBYTES + message.size());
		crypto_box_seal(*encryptedMessage, (const unsigned char*)message.data(), message.size(), keys->getPublicKey());
		return encryptedMessage;
	}

	std::string decrypt(const AuthenticatedEncryption* keys, const MemoryBin* encryptedMessage)
	{
		assert(keys && keys->getPrivateKey() && encryptedMessage);
		// int crypto_box_seal_open(unsigned char* m, const unsigned char* c,
		//     unsigned long long clen,
		// 	   const unsigned char* pk, const unsigned char* sk);
		auto mm = MemoryManager::getInstance();
		MemoryBin* temp = mm->getMemory(encryptedMessage->size() - crypto_box_SEALBYTES);
		memset(*temp, 0, temp->size());
		int result = crypto_box_seal_open(*temp, *encryptedMessage, encryptedMessage->size(),
			keys->getPublicKey(), *keys->getPrivateKey());
			
		std::string clearMessage((const char*)temp->data(), temp->size());
		if (result) {
			printf("box seal open result: %d\n", result);
			printf("clearMessage: %s\n", clearMessage.data());
			printf("public key: %s\n", DataTypeConverter::binToHex(std::string((const char*)keys->getPublicKey(), keys->getPublicKeySize())));
			printf("private key: %s\n", DataTypeConverter::binToHex(keys->getPrivateKey()).data());
			throw DecryptException("couldn't decrypt message");
		}
		mm->releaseMemory(temp);
		return std::move(clearMessage);
	}
}
