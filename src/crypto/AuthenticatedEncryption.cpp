#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

AuthenticatedEncryption::AuthenticatedEncryption()
	: mPrivkey(nullptr), mPrecalculatedSharedSecretLastIndex(0)
{
	mPrivkey = MemoryManager::getInstance()->getMemory(crypto_scalarmult_curve25519_BYTES);
	crypto_box_keypair(mPubkey, *mPrivkey);
}

AuthenticatedEncryption::AuthenticatedEncryption(KeyPairEd25519* ed25519KeyPair)
	: mPrivkey(nullptr), mPrecalculatedSharedSecretLastIndex(0)
{
	if (ed25519KeyPair->getPrivateKey()) {
		mPrivkey = MemoryManager::getInstance()->getMemory(crypto_scalarmult_curve25519_BYTES);
		crypto_sign_ed25519_sk_to_curve25519(mPrivkey->data(), ed25519KeyPair->getPrivateKey()->data());
		crypto_scalarmult_base(mPubkey, *mPrivkey);
	}
	else if (ed25519KeyPair->getPublicKey()) {
		crypto_sign_ed25519_pk_to_curve25519(mPubkey, ed25519KeyPair->getPublicKey());
	}
}

AuthenticatedEncryption::AuthenticatedEncryption(MemoryBin* privateKeyx25519)
	: mPrivkey(privateKeyx25519), mPrecalculatedSharedSecretLastIndex(0)
{
	crypto_scalarmult_base(mPubkey, *mPrivkey);
}

AuthenticatedEncryption::AuthenticatedEncryption(const unsigned char pubkeyx25519[crypto_scalarmult_curve25519_BYTES])
	: mPrivkey(nullptr), mPrecalculatedSharedSecretLastIndex(0)
{
	memcpy(mPubkey, pubkeyx25519, crypto_scalarmult_curve25519_BYTES);
}

AuthenticatedEncryption::~AuthenticatedEncryption()
{
	auto mm = MemoryManager::getInstance();
	if (mPrivkey) {
		mm->releaseMemory(mPrivkey);
	}
	mPrecalculatedSharedSecretsMutex.lock();

	for (auto it = mPrecalculatedSharedSecrets.begin(); it != mPrecalculatedSharedSecrets.end(); it++) {
		mm->releaseMemory(it->second);
	}
	mPrecalculatedSharedSecrets.clear();
	mPrecalculatedSharedSecretsMutex.unlock();
}

MemoryBin* AuthenticatedEncryption::encrypt(const unsigned char* message, size_t messageSize, AuthenticatedEncryption* recipiantKey)
{
	if (!mPrivkey) return nullptr;
	auto result = MemoryManager::getInstance()->getMemory(crypto_box_NONCEBYTES + crypto_box_MACBYTES + messageSize);
	randombytes_buf(*result, crypto_box_NONCEBYTES);
	/*int crypto_box_easy(unsigned char* c, const unsigned char* m,
		unsigned long long mlen, const unsigned char* n,
		const unsigned char* pk, const unsigned char* sk);*/
	if (crypto_box_easy(&result->data()[crypto_box_NONCEBYTES], message, messageSize, *result, recipiantKey->mPubkey, *mPrivkey)) {
		throw AuthenticatedEncryptionException("error by encrypt message");
	}
	return result;
}

MemoryBin* AuthenticatedEncryption::encrypt(const MemoryBin* message, int precalculatedSharedSecretIndex)
{
	if (!mPrivkey) return nullptr;
	auto mm = MemoryManager::getInstance();
	auto result = mm->getMemory(crypto_box_NONCEBYTES + crypto_box_MACBYTES + message->size());
	int function_result = -1;
	randombytes_buf(*result, crypto_box_NONCEBYTES);

	mPrecalculatedSharedSecretsMutex.lock();
	auto sharedSecret = mPrecalculatedSharedSecrets.find(precalculatedSharedSecretIndex);
	if (sharedSecret != mPrecalculatedSharedSecrets.end()) {
		/*int crypto_box_easy_afternm(unsigned char* c, const unsigned char* m,
		unsigned long long mlen, const unsigned char* n,
		const unsigned char* k);*/
		function_result = crypto_box_easy_afternm(&result->data()[crypto_box_NONCEBYTES], message->data(),
			message->size(), *result,
			*(sharedSecret->second));
	} else {
		mm->releaseMemory(result);
		result = nullptr;
	}
	if (function_result) {
		mm->releaseMemory(result);
		result = nullptr;
	}
	mPrecalculatedSharedSecretsMutex.unlock();
	
	return result;
}

MemoryBin* AuthenticatedEncryption::decrypt(const MemoryBin* encryptedMessage, AuthenticatedEncryption* senderKey)
{
	if (!mPrivkey) return nullptr;
	auto mm = MemoryManager::getInstance();
	auto result = mm->getMemory(encryptedMessage->size() - crypto_box_NONCEBYTES - crypto_box_MACBYTES);

	/*int crypto_box_open_easy(unsigned char* m, const unsigned char* c,
		unsigned long long clen, const unsigned char* n,
		const unsigned char* pk, const unsigned char* sk);*/
	// The function returns -1 if the verification fails, and 0 on success. On success, the decrypted message is stored into m.
	if (crypto_box_open_easy(*result, &encryptedMessage->data()[crypto_box_NONCEBYTES],
		encryptedMessage->size() - crypto_box_NONCEBYTES, encryptedMessage->data(),
		senderKey->mPubkey, *mPrivkey)) {
		mm->releaseMemory(result);
		result = nullptr;
	}
	return result;
}

MemoryBin* AuthenticatedEncryption::decrypt(const MemoryBin* encryptedMessage, int precalculatedSharedSecretIndex)
{
	if (!mPrivkey) return nullptr;
	auto mm = MemoryManager::getInstance();
	auto result = mm->getMemory(encryptedMessage->size() - crypto_box_NONCEBYTES - crypto_box_MACBYTES);
	int function_result = -1;

	mPrecalculatedSharedSecretsMutex.lock();
	auto sharedSecret = mPrecalculatedSharedSecrets.find(precalculatedSharedSecretIndex);
	
	if (sharedSecret != mPrecalculatedSharedSecrets.end()) {
		/*int crypto_box_open_easy_afternm(unsigned char* m, const unsigned char* c,
		unsigned long long clen, const unsigned char* n,
		const unsigned char* k); */
		// The function returns -1 if the verification fails, and 0 on success. On success, the decrypted message is stored into m.
		function_result = crypto_box_open_easy_afternm(*result, &encryptedMessage->data()[crypto_box_NONCEBYTES],
			encryptedMessage->size() - crypto_box_NONCEBYTES, encryptedMessage->data(),
			*(sharedSecret->second));
	}
	mPrecalculatedSharedSecretsMutex.unlock();

	if (function_result) {
		mm->releaseMemory(result);
		result = nullptr;
	}
	return result;
}

int AuthenticatedEncryption::precalculateSharedSecret(AuthenticatedEncryption* recipiantKey)
{
	if (!mPrivkey) return -1;
	Poco::ScopedLock<Poco::FastMutex> _lock(mPrecalculatedSharedSecretsMutex);
	/*int crypto_box_beforenm(unsigned char* k, const unsigned char* pk,
		const unsigned char* sk);*/
	auto sharedSecret = MemoryManager::getInstance()->getMemory(crypto_box_BEFORENMBYTES);
	crypto_box_beforenm(*sharedSecret, recipiantKey->mPubkey, *mPrivkey);
	mPrecalculatedSharedSecretLastIndex++;
	mPrecalculatedSharedSecrets.insert({ mPrecalculatedSharedSecretLastIndex, sharedSecret });
	return mPrecalculatedSharedSecretLastIndex;
}

bool AuthenticatedEncryption::removePrecalculatedSharedSecret(int index)
{
	Poco::ScopedLock<Poco::FastMutex> _lock(mPrecalculatedSharedSecretsMutex);
	auto it = mPrecalculatedSharedSecrets.find(index);
	if (it != mPrecalculatedSharedSecrets.end()) {
		MemoryManager::getInstance()->releaseMemory(it->second);
		mPrecalculatedSharedSecrets.erase(it);
		return true;
	}
	return false;
}

