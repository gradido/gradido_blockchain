#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

AuthenticatedEncryption::AuthenticatedEncryption()
	: mPrecalculatedSharedSecretLastIndex(0)
{
	memory::Block pubkey(X25519_PUBLIC_KEY_SIZE);
	memory::Block privkey(X25519_PRIVATE_KEY_SIZE);

	crypto_box_keypair(pubkey, privkey);

	mPubkey = std::make_shared<memory::Block>(pubkey);
	mPrivkey = std::make_shared<memory::Block>(privkey);
}

AuthenticatedEncryption::AuthenticatedEncryption(KeyPairEd25519* ed25519KeyPair)
	: mPrecalculatedSharedSecretLastIndex(0)
{
	memory::Block pubkey(X25519_PUBLIC_KEY_SIZE);
	if (ed25519KeyPair->getPrivateKey()) {		
		memory::Block privkey(X25519_PRIVATE_KEY_SIZE);
		crypto_sign_ed25519_sk_to_curve25519(privkey, *ed25519KeyPair->getPrivateKey());
		crypto_scalarmult_base(pubkey, privkey);
		mPrivkey = std::make_shared<memory::Block>(privkey);
	}
	else if (ed25519KeyPair->getPublicKey()) {
		if(crypto_sign_ed25519_pk_to_curve25519(pubkey, *ed25519KeyPair->getPublicKey())) {
			throw AuthenticatedKeyTransformationException("error transforming ed25519 key to curve25519");
		}
	}
	mPubkey = std::make_shared<memory::Block>(pubkey);
}

AuthenticatedEncryption::AuthenticatedEncryption(memory::ConstBlockPtr privateKeyx25519)
	: mPrecalculatedSharedSecretLastIndex(0)
{
	memory::Block pubkey(X25519_PUBLIC_KEY_SIZE);
	memory::Block privkey(X25519_PRIVATE_KEY_SIZE);

	crypto_scalarmult_base(pubkey, privkey);

	mPubkey = std::make_shared<memory::Block>(pubkey);
	mPrivkey = std::make_shared<memory::Block>(privkey);
}

AuthenticatedEncryption::AuthenticatedEncryption(const std::array<unsigned char, X25519_PUBLIC_KEY_SIZE>& pubkeyx25519)
	: mPrecalculatedSharedSecretLastIndex(0), mPubkey(std::make_shared<memory::Block>(pubkeyx25519.size(), pubkeyx25519.data()))
{
}

AuthenticatedEncryption::~AuthenticatedEncryption()
{
	std::lock_guard _lock(mPrecalculatedSharedSecretsMutex);
	mPrecalculatedSharedSecrets.clear();
}

memory::Block AuthenticatedEncryption::encrypt(const unsigned char* message, size_t messageSize, AuthenticatedEncryption* recipiantKey)
{
	if (!mPrivkey) {
		throw AuthenticatedEncryptionException("encrypt called with empty private key");
	}

	memory::Block result(crypto_box_NONCEBYTES + crypto_box_MACBYTES + messageSize);
	randombytes_buf(result.data(), crypto_box_NONCEBYTES);
	/*int crypto_box_easy(unsigned char* c, const unsigned char* m,
		unsigned long long mlen, const unsigned char* n,
		const unsigned char* pk, const unsigned char* sk);*/
	if (crypto_box_easy(&result.data()[crypto_box_NONCEBYTES], message, messageSize, result.data(), *recipiantKey->mPubkey, mPrivkey->data())) {
		throw AuthenticatedEncryptionException("error by encrypt message");
	}
	return result;
}

memory::Block AuthenticatedEncryption::encrypt(const memory::Block& message, int precalculatedSharedSecretIndex)
{
	if (!mPrivkey) {
		throw AuthenticatedEncryptionException("encrypt with precalculated shared secret index called with empty private key");
	}
	
	memory::Block result(crypto_box_NONCEBYTES + crypto_box_MACBYTES + message.size());
	randombytes_buf(result.data(), crypto_box_NONCEBYTES);

	std::lock_guard _lock(mPrecalculatedSharedSecretsMutex);
	auto sharedSecret = mPrecalculatedSharedSecrets.find(precalculatedSharedSecretIndex);
	if (sharedSecret != mPrecalculatedSharedSecrets.end()) {
		/*int crypto_box_easy_afternm(unsigned char* c, const unsigned char* m,
		unsigned long long mlen, const unsigned char* n,
		const unsigned char* k);*/
		if(crypto_box_easy_afternm(&result.data()[crypto_box_NONCEBYTES], message.data(),
			message.size(), result.data(),
			sharedSecret->second->data())) {
				throw AuthenticatedEncryptionException("error by encrypt message");
			}
	} 	
	return result;
}

memory::Block AuthenticatedEncryption::decrypt(const memory::Block& encryptedMessage, AuthenticatedEncryption* senderKey)
{
	if (!mPrivkey) {
		throw AuthenticatedEncryptionException("decrypt called with empty private key");
	}
	memory::Block result(encryptedMessage.size() - crypto_box_NONCEBYTES - crypto_box_MACBYTES);

	/*int crypto_box_open_easy(unsigned char* m, const unsigned char* c,
		unsigned long long clen, const unsigned char* n,
		const unsigned char* pk, const unsigned char* sk);*/
	// The function returns -1 if the verification fails, and 0 on success. On success, the decrypted message is stored into m.
	if (crypto_box_open_easy(result.data(), &encryptedMessage.data()[crypto_box_NONCEBYTES],
		encryptedMessage.size() - crypto_box_NONCEBYTES, encryptedMessage.data(),
		*senderKey->mPubkey, mPrivkey->data())) {
			throw AuthenticatedDecryptionException("error by decrypt message");
	}
	return result;
}

memory::Block AuthenticatedEncryption::decrypt(const memory::Block& encryptedMessage, int precalculatedSharedSecretIndex)
{
	if (!mPrivkey) {
		throw AuthenticatedEncryptionException("decrypt with precalculated shared secret index called with empty private key");
	}
	memory::Block result(encryptedMessage.size() - crypto_box_NONCEBYTES - crypto_box_MACBYTES);

	std::lock_guard _lock(mPrecalculatedSharedSecretsMutex);
	auto sharedSecret = mPrecalculatedSharedSecrets.find(precalculatedSharedSecretIndex);
	
	if (sharedSecret != mPrecalculatedSharedSecrets.end()) {
		/*int crypto_box_open_easy_afternm(unsigned char* m, const unsigned char* c,
		unsigned long long clen, const unsigned char* n,
		const unsigned char* k); */
		// The function returns -1 if the verification fails, and 0 on success. On success, the decrypted message is stored into m.
		if(crypto_box_open_easy_afternm(result.data(), &encryptedMessage.data()[crypto_box_NONCEBYTES],
			encryptedMessage.size() - crypto_box_NONCEBYTES, encryptedMessage.data(),
			sharedSecret->second->data())) {
				throw AuthenticatedDecryptionException("error by decrypt message");
			}
	}

	return result;
}

int AuthenticatedEncryption::precalculateSharedSecret(AuthenticatedEncryption* recipiantKey)
{
	if (!mPrivkey) return -1;
	std::lock_guard _lock(mPrecalculatedSharedSecretsMutex);
	/*int crypto_box_beforenm(unsigned char* k, const unsigned char* pk,
		const unsigned char* sk);*/
	auto sharedSecret = std::make_unique<memory::Block>(crypto_box_BEFORENMBYTES);
	if(crypto_box_beforenm(sharedSecret->data(), *recipiantKey->mPubkey, mPrivkey->data())) {
		throw AuthenticatedPrepareException("error in precalculateSharedSecret");
	};
	mPrecalculatedSharedSecretLastIndex++;
	mPrecalculatedSharedSecrets.insert({ mPrecalculatedSharedSecretLastIndex, std::move(sharedSecret) });
	return mPrecalculatedSharedSecretLastIndex;
}

bool AuthenticatedEncryption::removePrecalculatedSharedSecret(int index)
{
	std::lock_guard _lock(mPrecalculatedSharedSecretsMutex);
	auto it = mPrecalculatedSharedSecrets.find(index);
	if (it != mPrecalculatedSharedSecrets.end()) {
		mPrecalculatedSharedSecrets.erase(it);
		return true;
	}
	return false;
}

