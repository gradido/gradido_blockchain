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

#include "gradido_blockchain/memory/Block.h"
#include "sodium.h"
#include "SecretKeyCryptography.h"
#include "Passphrase.h"
#include "../lib/DataTypeConverter.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

enum class Ed25519DerivationType {
	SOFT,
	HARD
};

class KeyPairEd25519Ex;
#define ED25519_PRIVATE_KEY_SIZE crypto_sign_SECRETKEYBYTES
#define ED25519_PUBLIC_KEY_SIZE crypto_sign_PUBLICKEYBYTES
#define ED25519_SIGNATURE_SIZE crypto_sign_BYTES
#define ED25519_CHAIN_CODE_SIZE 32

using namespace memory;

/*
* ed25519-bip32 based on: https://input-output-hk.github.io/adrestia/static/Ed25519_BIP.pdf and https://github.com/typed-io/rust-ed25519-bip32
*/

class GRADIDOBLOCKCHAIN_EXPORT KeyPairEd25519
{
	friend class AuthenticatedEncryption;
public:
	//! \param private key = extended ed25519 secret, containing the normalized hash of seed only
	KeyPairEd25519(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr privateKey = nullptr, memory::ConstBlockPtr chainCode = nullptr);
	virtual ~KeyPairEd25519();

	//! \param passphrase must contain word indices
	static std::shared_ptr<KeyPairEd25519> create(const std::shared_ptr<Passphrase> passphrase);
	//! create ed25519 bip32 compatible key pair
	//! \param seed 32 Byte seed
	static std::shared_ptr<KeyPairEd25519> create(const memory::Block& seed);
	static memory::Block calculatePublicKey(const memory::Block& privateKey);

	std::shared_ptr<KeyPairEd25519Ex> deriveChild(uint32_t index) const;
	static Ed25519DerivationType getDerivationType(uint32_t index);

	memory::Block sign(const memory::Block& message) const { return sign(message.data(), message.size()); }
	inline memory::Block sign(const std::string& bodyBytes) const { return sign((const unsigned char*)bodyBytes.data(), bodyBytes.size()); }
	memory::Block sign(const unsigned char* message, size_t messageSize) const;

	//! \return true if signature is valid
	bool verify(const std::string& message, const std::string& signature) const;
	bool verify(const memory::Block& message, const memory::Block& signature) const;
	bool is3rdHighestBitClear() const;

	inline memory::ConstBlockPtr getPublicKey() const { return mSodiumPublic; }
	inline memory::ConstBlockPtr getChainCode() const { return mChainCode; }

	inline bool isTheSame(const KeyPairEd25519& b) const {
		return *b.mSodiumPublic == *mSodiumPublic;
	}
	inline bool isTheSame(const unsigned char* pubkey) const {
		if (!pubkey)
			return false;
		return 0 == sodium_memcmp(*mSodiumPublic, pubkey, ED25519_PUBLIC_KEY_SIZE);
	}
	//! \return 0 if the same
	//! \return -1 if not the same
	//! \return 1 if hasn't private key
	inline int isTheSame(memory::ConstBlockPtr privkey) const {
		if (!mExtendedSecret) return 1;
		if (privkey->size() != mExtendedSecret->size()) return -1;
		return sodium_memcmp(mExtendedSecret->data(), privkey->data(), privkey->size());
	}

	inline bool operator == (const KeyPairEd25519& b) const { return isTheSame(b);  }
	inline bool operator != (const KeyPairEd25519& b) const { return !isTheSame(b); }

	inline bool operator == (const unsigned char* b) const { return isTheSame(b); }
	inline bool operator != (const unsigned char* b) const { return !isTheSame(b); }

	inline bool hasPrivateKey() const { return static_cast<bool>(mExtendedSecret); }
	memory::Block getCryptedPrivKey(const SecretKeyCryptography& password) const;

	/// takes the given raw bytes and perform some modifications to normalize
	/// to a valid Ed25519 extended key, but it does also force
	/// the 3rd highest bit to be cleared too.
	static void normalizeBytesForce3rd(memory::Block& key);
	static bool isNormalized(const memory::Block& key);
	bool isNormalized();

protected:
	inline memory::ConstBlockPtr getPrivateKey() const { return mExtendedSecret; }
	//! check if all keys have the correct sizes (if present)
	//! throw if not
	void checkKeySizes();
	std::shared_ptr<KeyPairEd25519Ex> derivePrivateKey(uint32_t index) const;
	std::shared_ptr<KeyPairEd25519Ex> derivePublicKey(uint32_t index) const;

private:
	//!
	// 32 Byte
	//! \brief ed25519 libsodium public key
	memory::ConstBlockPtr mSodiumPublic;

	//! TODO: replace MemoryBin by a memory obfuscation class which make it hard to steal the private key from memory
	//! ed25519 extended private key
	//! contain only the normalized hash of the seed
	//! seed isn't known in childs
	memory::ConstBlockPtr mExtendedSecret;

	// 32 Byte
	memory::ConstBlockPtr mChainCode;
};

// *********************** Exceptions ****************************
class GRADIDOBLOCKCHAIN_EXPORT Ed25519SignException : public GradidoBlockchainException
{
public:
	explicit Ed25519SignException(const char* what, memory::ConstBlockPtr pubkey, const std::string& message) noexcept;
	Ed25519SignException(const char* what, const unsigned char* pubkey, const std::string& message) noexcept;
	~Ed25519SignException();
	std::string getFullString() const;

protected:
	memory::ConstBlockPtr mPubkey;
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
	explicit Ed25519DeriveException(const char* what, memory::ConstBlockPtr pubkey) noexcept;
	~Ed25519DeriveException();
	std::string getFullString() const;

protected:
	memory::ConstBlockPtr mPubkey;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519InvalidKeyException: public GradidoBlockchainException
{
public:
	//! \param invalidKey move key and free up memory on exception deconstruct
	explicit Ed25519InvalidKeyException(const char* what, const memory::Block& invalidKey, size_t expectedKeySize = 0) noexcept;
	~Ed25519InvalidKeyException();
	std::string getFullString() const;

protected:
	std::string mKeyHex;
	size_t mExpectedKeySize;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519MissingKeyException : public GradidoBlockchainException
{
public:
	//! \param invalidKey move key and free up memory on exception deconstruct
	explicit Ed25519MissingKeyException(const char* what) noexcept : GradidoBlockchainException(what) {};
	~Ed25519MissingKeyException() {};
	std::string getFullString() const { return what(); };
protected:
};


class GRADIDOBLOCKCHAIN_EXPORT ED25519InvalidPrivateKeyForPublicKey : public GradidoBlockchainException
{
public:
	explicit ED25519InvalidPrivateKeyForPublicKey(const char* what, std::string publicKey) noexcept;
	std::string getFullString() const;

protected:
	std::string mPublicKey;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519InvalidSeedException : public GradidoBlockchainException
{
public:
	explicit Ed25519InvalidSeedException(const char* what, const std::string& seedHex) noexcept;
	std::string getFullString() const;

protected:
	std::string mSeedHex;
};

#endif //__GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H
