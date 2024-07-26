#ifndef GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H
#define GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H

#include "sodium.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "rapidjson/document.h"

#include <map>
#include <mutex>

using namespace memory;

class KeyPairEd25519;

#define X25519_PUBLIC_KEY_SIZE crypto_scalarmult_curve25519_BYTES
#define X25519_PRIVATE_KEY_SIZE crypto_scalarmult_curve25519_BYTES

class GRADIDOBLOCKCHAIN_EXPORT AuthenticatedEncryption
{
public:
	AuthenticatedEncryption();
	AuthenticatedEncryption(KeyPairEd25519* ed25519KeyPair);
	AuthenticatedEncryption(memory::ConstBlockPtr privateKeyx25519);
	AuthenticatedEncryption(const std::array<unsigned char, X25519_PUBLIC_KEY_SIZE>& pubkeyx25519);
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

	//! return index for the shared secret for this recipiant public key
	int precalculateSharedSecret(AuthenticatedEncryption* recipiantKey);
	bool removePrecalculatedSharedSecret(int index);
	
	memory::ConstBlockPtr mPubkey;
	inline const memory::ConstBlockPtr getPublicKey() const { return mPubkey; }
	inline const memory::ConstBlockPtr getPrivateKey() const { return mPrivkey; }
	inline bool hasPrivateKey() const { return static_cast<bool>(mPrivkey); }
protected:
	
	memory::ConstBlockPtr mPrivkey;

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

class AuthenticatedDecryptionException : public GradidoBlockchainException
{
public:
	explicit AuthenticatedDecryptionException(const char* message) noexcept : GradidoBlockchainException(message) {}
	std::string getFullString() const {
		return what();
	}
};

class AuthenticatedKeyTransformationException : public GradidoBlockchainException
{
public:
	explicit AuthenticatedKeyTransformationException(const char* message) noexcept : GradidoBlockchainException(message) {}
	std::string getFullString() const {
		return what();
	}
};

class AuthenticatedPrepareException : public GradidoBlockchainException
{
public:
	explicit AuthenticatedPrepareException(const char* message) noexcept : GradidoBlockchainException(message) {}
	std::string getFullString() const {
		return what();
	}
};
#endif //GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H