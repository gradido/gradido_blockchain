#ifndef GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H
#define GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H

#include "sodium.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "rapidjson/document.h"

#include "Poco/Mutex.h"

#include <map>

class KeyPairEd25519;

class GRADIDOBLOCKCHAIN_EXPORT AuthenticatedEncryption
{
public:
	AuthenticatedEncryption();
	AuthenticatedEncryption(KeyPairEd25519* ed25519KeyPair);
	AuthenticatedEncryption(MemoryBin* privateKeyx25519);
	AuthenticatedEncryption(const unsigned char pubkeyx25519[crypto_scalarmult_curve25519_BYTES]);
	~AuthenticatedEncryption();

	MemoryBin* encrypt(const unsigned char* message, size_t messageSize, AuthenticatedEncryption* recipiantKey);
	inline MemoryBin* encrypt(MemoryBin* message, AuthenticatedEncryption* recipiantKey) {
		return encrypt(message->data(), message->size(), recipiantKey);
	}
	inline MemoryBin* encrypt(const std::string& message, AuthenticatedEncryption* recipiantKey) {
		return encrypt((const unsigned char*)message.data(), message.size(), recipiantKey);
	}

	MemoryBin* encrypt(const MemoryBin* message, int precalculatedSharedSecretIndex);
	MemoryBin* decrypt(const MemoryBin* encryptedMessage, AuthenticatedEncryption* senderKey);
	MemoryBin* decrypt(const MemoryBin* encryptedMessage, int precalculatedSharedSecretIndex);

	static size_t getPublicKeySize() { return crypto_scalarmult_curve25519_BYTES; }
	static size_t getPrivateKeySize() { return crypto_scalarmult_curve25519_BYTES; }

	//! return index for the shared secret for this recipiant public key
	int precalculateSharedSecret(AuthenticatedEncryption* recipiantKey);
	bool removePrecalculatedSharedSecret(int index);
	
	unsigned char mPubkey[crypto_scalarmult_curve25519_BYTES];
	inline const unsigned char* getPublicKey() const { return mPubkey; }
	inline const MemoryBin* getPrivateKey() const { return mPrivkey; }
protected:
	
	MemoryBin* mPrivkey;

	Poco::FastMutex mPrecalculatedSharedSecretsMutex;
	std::map<int, MemoryBin*> mPrecalculatedSharedSecrets;
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