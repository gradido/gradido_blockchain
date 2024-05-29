#include "gradido_blockchain/crypto/SecretKeyCryptography.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

#include "gradido_blockchain/lib/Profiler.h"

#include "sodium.h"
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

SecretKeyCryptography::ResultType SecretKeyCryptography::createKey(const std::string& salt_parameter, const std::string& passwd)
{
	assert(crypto_hash_sha512_BYTES >= crypto_pwhash_SALTBYTES);
		
	auto app_secret = CryptoConfig::g_CryptoAppSecret;
	if (!app_secret) {
		throw CryptoConfig::MissingKeyException("in SecretKeyCryptography::createKey key is missing", "crypto.app_secret");
	}
	if (!CryptoConfig::g_ServerCryptoKey) {
		throw CryptoConfig::MissingKeyException("in SecretKeyCryptography::createKey key is missing", "crypto.server_key");
	}
	
	Profiler timeUsed;
	std::unique_lock<std::shared_mutex> _lock(mWorkingMutex);
#ifndef _TEST_BUILD
	if (timeUsed.millis() > 10) {
		printf("[SecretKeyCryptography::createKey] wait %s on getting lock\n", timeUsed.string().data());
		timeUsed.reset();
	}
#endif 
	
	// use hash512 because existing data where calculated with that, but could be also changed to hash256
	memory::Block hash512_salt(crypto_hash_sha512_BYTES); // need at least crypto_pwhash_SALTBYTES 16U

	crypto_hash_sha512_state state;
	crypto_hash_sha512_init(&state);
	//crypto_hash_sha512_update
	crypto_hash_sha512_update(&state, (const unsigned char*)salt_parameter.data(), salt_parameter.size());
	crypto_hash_sha512_update(&state, app_secret->data(), app_secret->size());
	crypto_hash_sha512_final(&state, hash512_salt.data());

#ifndef _TEST_BUILD
	if (timeUsed.millis() > 200) {
		printf("[SecretKeyCryptography::createKey] %s calculating sha512\n", timeUsed.string().data());
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

		return AUTH_CREATE_ENCRYPTION_KEY_FAILED;
	}
#ifndef _TEST_BUILD
	if (timeUsed.millis() > 400) {
		printf("[SecretKeyCryptography::createKey] %s calculating pwd hash\n", timeUsed.string().data());
	}
#endif
	// generate hash from key for compare
	assert(sizeof(KeyHashed) >= crypto_shorthash_BYTES);
	assert(CryptoConfig::g_ServerCryptoKey);
	crypto_shorthash((unsigned char*)&mEncryptionKeyHash, mEncryptionKey->data(), crypto_box_SEEDBYTES, CryptoConfig::g_ServerCryptoKey->data());

	return AUTH_CREATE_ENCRYPTION_KEY_SUCCEED;
}

SecretKeyCryptography::ResultType SecretKeyCryptography::encrypt(const memory::Block& message, MemoryBlockPtr encryptedMessage) const
{
	assert(message && encryptedMessage);
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);

	if (!mEncryptionKey) {
		return AUTH_NO_KEY;
	}
	
	size_t message_len = message.size();
	size_t ciphertext_len = crypto_secretbox_MACBYTES + message_len;

	unsigned char nonce[crypto_secretbox_NONCEBYTES];
	// we use a hardcoded value for nonce
	// TODO: use a dynamic value, save it along with the other parameters
	memset(nonce, 31, crypto_secretbox_NONCEBYTES);

	auto ciphertext = std::make_shared<memory::Block>(ciphertext_len);

	if (0 != crypto_secretbox_easy(ciphertext->data(), message.data(), message_len, nonce, mEncryptionKey->data())) {
		return AUTH_ENCRYPT_MESSAGE_FAILED;
	}

	encryptedMessage.swap(ciphertext);

	return AUTH_ENCRYPT_OK;
}

SecretKeyCryptography::ResultType SecretKeyCryptography::decrypt(const unsigned char* encryptedMessage, size_t encryptedMessageSize, MemoryBlockPtr message) const
{
	assert(message);
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);

	if (!mEncryptionKey) {
		return AUTH_NO_KEY;
	}

	size_t decryptSize = encryptedMessageSize - crypto_secretbox_MACBYTES;
	//unsigned char* decryptBuffer = (unsigned char*)malloc(decryptSize);
	//ObfusArray* decryptedData = new ObfusArray(decryptSize);
	auto decryptedData = std::make_shared<memory::Block>(decryptSize);
	unsigned char nonce[crypto_secretbox_NONCEBYTES];
	// we use a hardcoded value for nonce
	// TODO: use a dynamic value, save it along with the other parameters
	memset(nonce, 31, crypto_secretbox_NONCEBYTES);

	if (crypto_secretbox_open_easy(decryptedData->data(), encryptedMessage, encryptedMessageSize, nonce, mEncryptionKey->data())) {
		return AUTH_DECRYPT_MESSAGE_FAILED;
	}
	message.swap(decryptedData);

	return AUTH_DECRYPT_OK;
}

const char* SecretKeyCryptography::getErrorMessage(ResultType type)
{
	switch (type) {
	case AUTH_ENCRYPT_OK: return "everything is ok";
	//case AUTH_ENCRYPT_SHA2_TO_SMALL: return "libsodium crypto_hash_sha512_BYTES is to small to use as crypto_pwhash_SALTBYTES";
	case AUTH_CREATE_ENCRYPTION_KEY_FAILED: return "error creating encryption key, maybe to much memory requested?";
	case AUTH_NO_KEY: return "no encryption key generated";
	case AUTH_ENCRYPT_MESSAGE_FAILED: return "message encryption failed";
	case AUTH_DECRYPT_MESSAGE_FAILED: return "message decryption failed";
	}
	return "<unknown>";
}