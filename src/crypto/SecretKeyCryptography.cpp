#include "gradido_blockchain/crypto/SecretKeyCryptography.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

#include "gradido_blockchain/lib/Profiler.h"

#include "sodium.h"
#include "loguru/loguru.hpp"

#include <assert.h>



SecretKeyCryptography::SecretKeyCryptography()
	: mOpsLimit(10), mMemLimit(33554432), mAlgo(2), mEncryptionKey(nullptr), mEncryptionKeyHash(0)
{
}

SecretKeyCryptography::SecretKeyCryptography(unsigned long long opslimit, size_t memlimit, int algo)
	: mOpsLimit(opslimit), mMemLimit(memlimit), mAlgo(algo), mEncryptionKey(nullptr), mEncryptionKeyHash(0)
{

}

SecretKeyCryptography::~SecretKeyCryptography()
{
}

void SecretKeyCryptography::createKey(const std::string& salt_parameter, const std::string& passwd)
{
	assert(crypto_hash_sha512_BYTES >= crypto_pwhash_SALTBYTES);

	auto appSecret = CryptoConfig::g_CryptoAppSecret;
	auto serverCryptoKey = CryptoConfig::g_ServerCryptoKey;
	if (!appSecret) {
		throw CryptoConfig::MissingKeyException("in SecretKeyCryptography::createKey key is missing", "crypto.app_secret");
	}
	if (!serverCryptoKey) {
		throw CryptoConfig::MissingKeyException("in SecretKeyCryptography::createKey key is missing", "crypto.server_key");
	}
#ifndef _TEST_BUILD
	Profiler timeUsed;
#endif
	std::unique_lock<std::shared_mutex> _lock(mWorkingMutex);
#ifndef _TEST_BUILD
	if (timeUsed.millis() > 10) {
		LOG_F(WARNING, "wait % s on getting lock\n", timeUsed.string().data());
		timeUsed.reset();
	}
#endif

	// use hash512 because existing data where calculated with that, but could be also changed to hash256
	memory::Block hash512_salt(crypto_hash_sha512_BYTES); // need at least crypto_pwhash_SALTBYTES 16U

	crypto_hash_sha512_state state;
	crypto_hash_sha512_init(&state);
	//crypto_hash_sha512_update
	crypto_hash_sha512_update(&state, (const unsigned char*)salt_parameter.data(), salt_parameter.size());
	crypto_hash_sha512_update(&state, appSecret->data(), appSecret->size());
	crypto_hash_sha512_final(&state, hash512_salt.data());

#ifndef _TEST_BUILD
	if (timeUsed.millis() > 200) {
		LOG_F(WARNING, "%s calculating sha512\n", timeUsed.string().data());
		timeUsed.reset();
	}
#endif

	//unsigned char* key = (unsigned char *)malloc(crypto_box_SEEDBYTES); // 32U
	//ObfusArray* key = new ObfusArray(crypto_box_SEEDBYTES);
	if (!mEncryptionKey) {
		mEncryptionKey = std::make_shared<memory::Block>(crypto_box_SEEDBYTES);
	}
	//Bin32Bytes* key = mm->get32Bytes();

	// generate encryption key, should take a bit longer to make brute force attacks hard
	if (crypto_pwhash(mEncryptionKey->data(), mEncryptionKey->size(), passwd.data(), passwd.size(), hash512_salt.data(), mOpsLimit, mMemLimit, mAlgo) != 0) {
		mEncryptionKey.reset();

		throw EncryptionKeyException("couldn't generate encryption key");
	}
#ifndef _TEST_BUILD
	if (timeUsed.millis() > 400) {
		LOG_F(WARNING, "%s calculating pwd hash\n", timeUsed.string().data());
	}
#endif
	// generate hash from key for compare
	assert(sizeof(KeyHashed) >= crypto_shorthash_BYTES);
	crypto_shorthash((unsigned char*)&mEncryptionKeyHash, mEncryptionKey->data(), crypto_box_SEEDBYTES, serverCryptoKey->data());
}

memory::Block SecretKeyCryptography::encrypt(const memory::Block& message) const
{
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);

	if (!mEncryptionKey) {
		throw MissingEncryptionException("key missing for encrypt");
	}

	size_t message_len = message.size();
	size_t ciphertext_len = crypto_secretbox_MACBYTES + message_len;

	unsigned char nonce[crypto_secretbox_NONCEBYTES];
	// we use a hardcoded value for nonce
	// TODO: use a dynamic value, save it along with the other parameters
	memset(nonce, 31, crypto_secretbox_NONCEBYTES);

	memory::Block ciphertext(ciphertext_len);

	if (0 != crypto_secretbox_easy(ciphertext, message, message_len, nonce, mEncryptionKey->data())) {
		throw EncryptionException("encrypt message failed");
	}

	return ciphertext;
}

memory::Block SecretKeyCryptography::decrypt(const unsigned char* encryptedMessage, size_t encryptedMessageSize) const
{
	assert(encryptedMessage && encryptedMessageSize);
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);

	if (!mEncryptionKey) {
		throw MissingEncryptionException("key missing for decrypt");
	}

	size_t decryptSize = encryptedMessageSize - crypto_secretbox_MACBYTES;
	//unsigned char* decryptBuffer = (unsigned char*)malloc(decryptSize);
	//ObfusArray* decryptedData = new ObfusArray(decryptSize);
	memory::Block decryptedData(decryptSize);
	unsigned char nonce[crypto_secretbox_NONCEBYTES];
	// we use a hardcoded value for nonce
	// TODO: use a dynamic value, save it along with the other parameters
	memset(nonce, 31, crypto_secretbox_NONCEBYTES);

	if (crypto_secretbox_open_easy(decryptedData, encryptedMessage, encryptedMessageSize, nonce, mEncryptionKey->data())) {
		throw DecryptionException("decrypt message failed", std::make_shared<memory::Block>(encryptedMessageSize, encryptedMessage));
	}

	return decryptedData;
}

DecryptionException::DecryptionException(const char* what, memory::ConstBlockPtr message) noexcept
	: SecretKeyCryptographyException(what), mMessage(message)
{

}

std::string DecryptionException::getFullString() const
{
	std::string result = what();
	result += "message converted to hex: " + mMessage->convertToHex();
	return result;
}


EncryptionKeyException::EncryptionKeyException(const char* what) noexcept
	: SecretKeyCryptographyException(what), mDetails(strerror(errno))
{
}

std::string EncryptionKeyException::getFullString() const
{
	std::string result = what();
	result += ", details: " + mDetails;
	return result;
}
