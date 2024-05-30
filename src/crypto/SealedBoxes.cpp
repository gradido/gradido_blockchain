#include "gradido_blockchain/crypto/SealedBoxes.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "loguru/loguru.hpp"

namespace SealedBoxes {
	memory::Block encrypt(const AuthenticatedEncryption* keys, const std::string& message)
	{
		assert(keys);
		//int crypto_box_seal(unsigned char* c, const unsigned char* m, unsigned long long mlen, const unsigned char* pk);
		memory::Block encryptedMessage(crypto_box_SEALBYTES + message.size());
		crypto_box_seal(encryptedMessage, (const unsigned char*)message.data(), message.size(), *keys->getPublicKey());
		return encryptedMessage;
	}

	std::string decrypt(const AuthenticatedEncryption* keys, const memory::Block& encryptedMessage)
	{
		assert(keys && keys->hasPrivateKey());
		// int crypto_box_seal_open(unsigned char* m, const unsigned char* c,
		//     unsigned long long clen,
		// 	   const unsigned char* pk, const unsigned char* sk);
		memory::Block temp(encryptedMessage.size() - crypto_box_SEALBYTES);

		int error = crypto_box_seal_open(temp, encryptedMessage, encryptedMessage.size(),
			*keys->getPublicKey(), *keys->getPrivateKey());
			
		std::string clearMessage((const char*)temp.data(), temp.size());
		if (error) {
			LOG_F(ERROR, "box seal open result: %d", error);
			LOG_F(ERROR, "clearMessage: %s", clearMessage.data());
			LOG_F(ERROR, "public key: %s", keys->getPublicKey()->convertToHexString().data());
			LOG_F(ERROR, "private key: %s\n", keys->getPrivateKey()->convertToHexString().data());
			throw DecryptException("couldn't decrypt message");
		}
		return clearMessage;
	}
}
