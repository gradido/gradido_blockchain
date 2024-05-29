#ifndef GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H
#define GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H

#include "sodium.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "rapidjson/document.h"

#include <map>
#include <mutex>

class KeyPairEd25519;

class GRADIDOBLOCKCHAIN_EXPORT AuthenticatedEncryption
{
public:
	AuthenticatedEncryption();
	AuthenticatedEncryption(KeyPairEd25519* ed25519KeyPair);
	AuthenticatedEncryption(const memory::Block& privateKeyx25519);
	AuthenticatedEncryption(const unsigned char pubkeyx25519[crypto_scalarmult_curve25519_BYTES]);
	~AuthenticatedEncryption();

	memory::Block encrypt(const unsigned char* message, size_t messageSize, AuthenticatedEncryption* recipiantKey);
	inline memory::Block encrypt(const memory::Block& message, AuthenticatedEncryption* recipiantKey) {
		return encrypt(message.data(), message.size(), recipiantKey);
	}
	inline memory::Block encrypt(const std::string& message, AuthenticatedEncryption* recipiantKey) {
		return encrypt((const unsigned char*)message.data(), message.size(), recipiantKey);
	}

	memory::Block encrypt(const memory::Block& message, int precalculatedSharedSecretIndex);
	memory::Block decrypt(const memory::Block& encryptedMessage, AuthenticatedEncryption* senderKey);
	memory::Block decrypt(const memory::Block& encryptedMessage, int precalculatedSharedSecretIndex);

	static size_t getPublicKeySize() { return crypto_scalarmult_curve25519_BYTES; }
	static size_t getPrivateKeySize() { return crypto_scalarmult_curve25519_BYTES; }

	//! return index for the shared secret for this recipiant public key
	int precalculateSharedSecret(AuthenticatedEncryption* recipiantKey);
	bool removePrecalculatedSharedSecret(int index);
	
	unsigned char mPubkey[crypto_scalarmult_curve25519_BYTES];
	inline const unsigned char* getPublicKey() const { return mPubkey; }
	inline const MemoryBlockPtr getPrivateKey() const { return mPrivkey; }
protected:
	
	MemoryBlockPtr mPrivkey;

	std::mutex mPrecalculatedSharedSecretsMutex;
	std::map<int, std::unique_ptr<memory::Block>> mPrecalculatedSharedSecrets;
	int mPrecalculatedSharedSecretLastIndex;
};

class AuthenticatedEncryptionException : public GradidoBlockchainException
{
public:
	explicit AuthenticatedEncryptionException(const char* message) noexcept : GradidoBlockchainException(message) {}
	std::string getFullString() const {
		return what();
	}
};


#endif //GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H