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
#define ED25519_PUBLIC_KEY_SIZE crypto_sign_PUBLICKEYBYTES
#define ED25519_PRIVATE_KEY_SIZE crypto_sign_SECRETKEYBYTES
#define ED25519_CHAIN_CODE_SIZE 32
#define ED25519_SIGNATURE_SIZE crypto_sign_BYTES

class GRADIDOBLOCKCHAIN_EXPORT KeyPairEd25519
{
	friend class AuthenticatedEncryption;
public:
	KeyPairEd25519(ConstMemoryBlockPtr publicKey, ConstMemoryBlockPtr privateKey = nullptr, ConstMemoryBlockPtr chainCode = nullptr);
	~KeyPairEd25519();

	//! \param passphrase must contain word indices
	static std::shared_ptr<KeyPairEd25519> create(const std::shared_ptr<Passphrase> passphrase);
	static memory::Block calculatePublicKey(ConstMemoryBlockPtr privateKey);

	std::shared_ptr<KeyPairEd25519Ex> deriveChild(uint32_t index);
	static Ed25519DerivationType getDerivationType(uint32_t index);

	memory::Block sign(const memory::Block& message) const { return sign(message.data(), message.size()); }
	inline memory::Block sign(const std::string& bodyBytes) const { return sign((const unsigned char*)bodyBytes.data(), bodyBytes.size()); }
	memory::Block sign(const unsigned char* message, size_t messageSize) const;

	//! \return true if signature is valid
	bool verify(const std::string& message, const std::string& signature) const;
	bool verify(const memory::Block& message, const memory::Block& signature) const;
	virtual bool is3rdHighestBitClear() const;

	inline ConstMemoryBlockPtr getPublicKey() const { return mSodiumPublic; }
	inline ConstMemoryBlockPtr getChainCode() const { return mChainCode; }

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
	inline int isTheSame(ConstMemoryBlockPtr privkey) const {
		if (!mSodiumSecret) return 1;
		if (privkey->size() != mSodiumSecret->size()) return -1;
		return sodium_memcmp(mSodiumSecret->data(), privkey->data(), privkey->size());
	}

	inline bool operator == (const KeyPairEd25519& b) const { return isTheSame(b);  }
	inline bool operator != (const KeyPairEd25519& b) const { return !isTheSame(b); }

	inline bool operator == (const unsigned char* b) const { return isTheSame(b); }
	inline bool operator != (const unsigned char* b) const { return !isTheSame(b); }

	inline bool hasPrivateKey() const { return static_cast<bool>(mSodiumSecret); }
	memory::Block getCryptedPrivKey(const std::shared_ptr<SecretKeyCryptography> password) const;

protected:
	inline ConstMemoryBlockPtr getPrivateKey() const { return mSodiumSecret; }
	//! check if all keys have the correct sizes (if present)
	//! throw if not
	void checkKeySizes();
private:
	//!
	// 32 Byte
	//! \brief ed25519 libsodium public key
	ConstMemoryBlockPtr mSodiumPublic;

	//! TODO: replace MemoryBin by a memory obfuscation class which make it hard to steal the private key from memory
	//! // 64 Byte
	//! \brief ed25519 libsodium private key
	ConstMemoryBlockPtr mSodiumSecret;

	// 32 Byte
	ConstMemoryBlockPtr mChainCode;
};

// *********************** Exceptions ****************************
class GRADIDOBLOCKCHAIN_EXPORT Ed25519SignException : public GradidoBlockchainException
{
public:
	explicit Ed25519SignException(const char* what, ConstMemoryBlockPtr pubkey, const std::string& message) noexcept;
	Ed25519SignException(const char* what, const unsigned char* pubkey, const std::string& message) noexcept;
	~Ed25519SignException();
	std::string getFullString() const;

protected:
	ConstMemoryBlockPtr mPubkey;
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
	explicit Ed25519DeriveException(const char* what, ConstMemoryBlockPtr pubkey) noexcept;
	~Ed25519DeriveException();
	std::string getFullString() const;

protected:
	ConstMemoryBlockPtr mPubkey;
};

class GRADIDOBLOCKCHAIN_EXPORT Ed25519InvalidKeyException: public GradidoBlockchainException
{
public:
	//! \param invalidKey move key and free up memory on exception deconstruct
	explicit Ed25519InvalidKeyException(const char* what, ConstMemoryBlockPtr invalidKey, size_t expectedKeySize = 0) noexcept;
	~Ed25519InvalidKeyException();
	std::string getFullString() const;

protected:
	ConstMemoryBlockPtr mKey;
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

#endif //__GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H
