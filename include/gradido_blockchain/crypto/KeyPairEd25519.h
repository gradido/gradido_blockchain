#ifndef __GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H
#define __GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H


/*!
 * \author: Dario Rekowski
 *
 * \date: 2020-06-04
 *
 * \brief: Key Pairs class for ed25519 keys, used for default gradido transactions
 * TODO: add verify method
*/


#include "sodium.h"
#include "SecretKeyCryptography.h"
#include "Passphrase.h"
#include "../lib/DataTypeConverter.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

enum DerivationType {
	DERIVATION_SOFT,
	DERIVATION_HARD
};

class KeyPairEd25519Ex;
class GRADIDOBLOCKCHAIN_EXPORT KeyPairEd25519
{
public:
	//! \param privateKey: take ownership, release after object destruction
	//! \param publicKey: copy
	KeyPairEd25519(MemoryBin* privateKey, MemoryBin* chainCode = nullptr);
	KeyPairEd25519(const unsigned char* publicKey, MemoryBin* chainCode = nullptr);

	~KeyPairEd25519();

	//! \param passphrase must contain word indices
	//! \return create KeyPairEd25519, caller muss call delete at return after finish
	static std::unique_ptr<KeyPairEd25519> create(const std::shared_ptr<Passphrase> passphrase);

	KeyPairEd25519Ex* deriveChild(Poco::UInt32 index);
	static DerivationType getDerivationType(Poco::UInt32 index);

	//! \return caller take ownership of return value
	MemoryBin* sign(const MemoryBin* message) const { return sign(message->data(), message->size()); }
	inline MemoryBin* sign(const std::string& bodyBytes) const { return sign((const unsigned char*)bodyBytes.data(), bodyBytes.size()); }
	MemoryBin* sign(const unsigned char* message, size_t messageSize) const;

	//! \return true if signature is valid
	bool verify(const std::string& message, const std::string& signature) const;
	bool verify(const MemoryBin* message, const MemoryBin* signature) const;
	virtual bool is3rdHighestBitClear() const;

	inline const unsigned char* getPublicKey() const { return mSodiumPublic; }
	inline MemoryBin* getChainCode() const { return mChainCode; }
	MemoryBin* getPublicKeyCopy() const;
	inline std::string getPublicKeyHex() const { return DataTypeConverter::binToHex(mSodiumPublic, getPublicKeySize()); }
	inline std::string getChainCodeHex() const { return DataTypeConverter::binToHex(mChainCode); }
	const static size_t getPublicKeySize() { return crypto_sign_PUBLICKEYBYTES; }

	inline bool isTheSame(const KeyPairEd25519& b) const {
		return 0 == sodium_memcmp(mSodiumPublic, b.mSodiumPublic, crypto_sign_PUBLICKEYBYTES);
	}
	inline bool isTheSame(const unsigned char* pubkey) const {
		if (!pubkey)
			return false;
		return 0 == sodium_memcmp(mSodiumPublic, pubkey, crypto_sign_PUBLICKEYBYTES);
	}
	//! \return 0 if the same
	//! \return -1 if not the same
	//! \return 1 if hasn't private key
	inline int isTheSame(const MemoryBin* privkey) const {
		if (!mSodiumSecret) return 1;
		if (privkey->size() != mSodiumSecret->size()) return -1;
		return sodium_memcmp(*mSodiumSecret, *privkey, privkey->size());
	}

	inline bool operator == (const KeyPairEd25519& b) const { return isTheSame(b);  }
	inline bool operator != (const KeyPairEd25519& b) const { return !isTheSame(b); }

	inline bool operator == (const unsigned char* b) const { return isTheSame(b); }
	inline bool operator != (const unsigned char* b) const { return !isTheSame(b); }

	inline bool hasPrivateKey() const { return mSodiumSecret != nullptr; }
	inline const MemoryBin* getPrivateKey() const { return mSodiumSecret; }

	MemoryBin* getCryptedPrivKey(const std::shared_ptr<SecretKeyCryptography> password) const;

protected:

	KeyPairEd25519();


private:
	// 64 Byte
	//! \brief ed25519 libsodium private key
	//!
	//! Why it is a pointer and the public is an array?
	//! Because MemoryBin should be replaced by a memory obfuscation class which make it harder to steal the private key from computer memory
	//! And because private key can be nullptr for example to verify a signed message

	//! TODO: replace MemoryBin by a memory obfuscation class which make it hard to steal the private key from memory
	MemoryBin* mSodiumSecret;
	MemoryBin* mChainCode;

	// 32 Byte
	//! \brief ed25519 libsodium public key
	unsigned char mSodiumPublic[crypto_sign_PUBLICKEYBYTES];
};

// *********************** Exceptions ****************************
class GRADIDOBLOCKCHAIN_EXPORT Ed25519SignException : public GradidoBlockchainException
{
public:
	explicit Ed25519SignException(const char* what, MemoryBin* pubkey, const std::string& message) noexcept;
	explicit Ed25519SignException(const char* what, const unsigned char* pubkey, const std::string& message) noexcept;
	~Ed25519SignException();
	std::string getFullString() const;

protected:
	MemoryBin* mPubkey;
	std::string mMessage;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519VerifyException: public GradidoBlockchainException
{
public:
	explicit Ed25519VerifyException(const char* what, std::string messageString, std::string signatureHex) noexcept;
	std::string getFullString() const;
	rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

protected:
	std::string mMessageString;
	std::string mSignatureHex;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519DeriveException : public GradidoBlockchainException
{
public:
	explicit Ed25519DeriveException(const char* what, MemoryBin* pubkey) noexcept;
	~Ed25519DeriveException();
	std::string getFullString() const;

protected:
	MemoryBin* mPubkey;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519InvalidKeyException: public GradidoBlockchainException
{
public:
	//! \param invalidKey move key and free up memory on exception deconstruct
	explicit Ed25519InvalidKeyException(const char* what, MemoryBin* invalidKey, size_t expectedKeySize = 0) noexcept;
	~Ed25519InvalidKeyException();
	std::string getFullString() const;

protected:
	MemoryBin* mKey;
	size_t mExpectedKeySize;
};


class GRADIDOBLOCKCHAIN_EXPORT ED25519InvalidPrivateKeyForPublicKey : public GradidoBlockchainException
{
public:
	explicit ED25519InvalidPrivateKeyForPublicKey(const char* what, std::string publicKey) noexcept;
	std::string getFullString() const;

protected:
	std::string mPublicKey;
};

#endif //__GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H
