#ifndef __GRADIDO_LOGIN_SERVER_CRYPTO_SECRET_KEY_CRYPTOGRAPHY_H
#define __GRADIDO_LOGIN_SERVER_CRYPTO_SECRET_KEY_CRYPTOGRAPHY_H

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Block.h"
#include <shared_mutex>
#include <vector>
#include <mutex>
#include <memory>


/*! 
 * 
 * \author: Dario Rekowski
 * 
 * \date: 07-06-2020
 *
 * \brief: Wrapper Class for make using libsodium secret key encryption easy, used for encrypt private keys for user with pwhash
 *
 */

typedef uint64_t KeyHashed;

class GRADIDOBLOCKCHAIN_EXPORT SecretKeyCryptography
{
public:

	//! \brief init with default algorithms parameter
	SecretKeyCryptography();
	//! \brief init with custom algorithms parameter
	//! 
	//! details see in libsodium crypto_pwhash 
	SecretKeyCryptography(unsigned long long opslimit, size_t memlimit, int algo);

	~SecretKeyCryptography();


	inline KeyHashed getKeyHashed() const { std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);  return mEncryptionKeyHash; }
	inline bool operator == (const std::shared_ptr<SecretKeyCryptography>& b) const {
		return isTheSame(b);
	}
	inline bool isTheSame(const std::shared_ptr<SecretKeyCryptography>& b) const {
		std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
		if (!b) return false;
		return mEncryptionKeyHash == b->getKeyHashed();
	}
	inline bool operator == (const KeyHashed& hash) const {
		return mEncryptionKeyHash == hash;
	}

	inline bool hasKey() const { std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);  return mEncryptionKey != nullptr; }

	//! \brief generate encryption key, with default parameter use ca. 300 ms 
	//! 
	//! should be call from task, running in g_CryptoCPUScheduler, lock shared mutex for writing
	//! \param salt_parameter for example email 
	void createKey(const std::string& salt_parameter, const std::string& passwd);

	memory::Block encrypt(const memory::Block& message) const;

	memory::Block decrypt(const memory::Block& encryptedMessage) const {
		return decrypt(encryptedMessage, encryptedMessage.size());
	}
	//! \brief same as the other decrypt only in other format
	//! \param encryptedMessage format from Poco Binary Data from DB, like returned from model/table/user for encrypted private key
	memory::Block decrypt(const std::vector<unsigned char>& encryptedMessage) const {
		return decrypt(encryptedMessage.data(), encryptedMessage.size());
	}
	//! \brief raw decrypt function, actual implementation
	memory::Block decrypt(const unsigned char* encryptedMessage, size_t encryptedMessageSize) const;

protected:
	// algorithms parameter
	unsigned long long mOpsLimit;
	size_t			   mMemLimit;
	int				   mAlgo;

	// encryption key and hash
	memory::BlockPtr mEncryptionKey;
	KeyHashed  mEncryptionKeyHash;

	mutable std::shared_mutex mWorkingMutex;
};

class GRADIDOBLOCKCHAIN_EXPORT SecretKeyCryptographyException : public GradidoBlockchainException
{
public:
	explicit SecretKeyCryptographyException(const char* what) noexcept 
		: GradidoBlockchainException(what) {}
	std::string getFullString() const { return what(); }
};

class GRADIDOBLOCKCHAIN_EXPORT MissingEncryptionException : public SecretKeyCryptographyException
{
public:
	explicit MissingEncryptionException(const char* what) noexcept 
		: SecretKeyCryptographyException(what) {}
};

// don't reveal message with should be encrypted on error
class GRADIDOBLOCKCHAIN_EXPORT EncryptionException : public SecretKeyCryptographyException
{
public:
	explicit EncryptionException(const char* what) noexcept 
		: SecretKeyCryptographyException(what) {}
};

class GRADIDOBLOCKCHAIN_EXPORT DecryptionException : public SecretKeyCryptographyException
{
public:
	explicit DecryptionException(const char* what, memory::ConstBlockPtr message) noexcept;
	std::string getFullString() const;

protected:
	memory::ConstBlockPtr mMessage;
};

class GRADIDOBLOCKCHAIN_EXPORT EncryptionKeyException : public SecretKeyCryptographyException
{
public:
	explicit EncryptionKeyException(const char* what) noexcept;
	std::string getFullString() const;
protected:
	std::string mDetails;
};


#endif //__GRADIDO_LOGIN_SERVER_CRYPTO_SECRET_KEY_CRYPTOGRAPHY_H
