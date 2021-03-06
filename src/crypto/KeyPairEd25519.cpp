
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include <assert.h>
#include "gradido_blockchain/lib/DataTypeConverter.h"



KeyPairEd25519::KeyPairEd25519(MemoryBin* privateKey)
	: mSodiumSecret(privateKey)
{
	//memcpy(mSodiumPublic, publicKey, crypto_sign_PUBLICKEYBYTES);
	// read pubkey from private key, so we are sure it is the correct pubkey for the private key

	crypto_sign_ed25519_sk_to_pk(mSodiumPublic, *privateKey);

}

KeyPairEd25519::KeyPairEd25519(const unsigned char* publicKey)
	: mSodiumSecret(nullptr)
{
	memcpy(mSodiumPublic, publicKey, crypto_sign_PUBLICKEYBYTES);
}

KeyPairEd25519::KeyPairEd25519()
	: mSodiumSecret(nullptr)
{
	memset(mSodiumPublic, 0, crypto_sign_PUBLICKEYBYTES);
}

KeyPairEd25519::~KeyPairEd25519()
{
	if (mSodiumSecret) {
		MemoryManager::getInstance()->releaseMemory(mSodiumSecret);
		mSodiumSecret = nullptr;
	}
}

std::unique_ptr<KeyPairEd25519> KeyPairEd25519::create(const std::shared_ptr<Passphrase> passphrase)
{
	//auto er = ErrorManager::getInstance();
	auto mm = MemoryManager::getInstance();
	assert(passphrase);
	// libsodium doc: https://libsodium.gitbook.io/doc/advanced/hmac-sha2
	// https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki
	
	auto word_indices = passphrase->getWordIndices();
	
	if (!word_indices || (!word_indices[0] && !word_indices[1] && !word_indices[2] && !word_indices[3])) {
		return nullptr;
	}
	std::string clear_passphrase = passphrase->createClearPassphrase();
	

	unsigned char hash[crypto_hash_sha512_BYTES];

	crypto_hash_sha512_state state;
	crypto_hash_sha512_init(&state);

	// ****  convert word indices into uint64  ****
	// To prevent breaking existing passphrase-hash combinations word indices will be put into 64 Bit Variable to mimic first implementation of algorithms
	auto valueSize = sizeof(Poco::UInt64);
	Poco::UInt64 value = 0;
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		value = word_indices[i];
		crypto_hash_sha512_update(&state, (const unsigned char*)&value, valueSize);
	}	
	// **** end converting into uint64 *****
	crypto_hash_sha512_update(&state, (unsigned char*)clear_passphrase.data(), clear_passphrase.size());
	crypto_hash_sha512_final(&state, hash);
	
	/*
	// debug passphrase
	printf("\passsphrase: <%s>\n", passphrase);
	printf("size word indices: %u\n", word_indices->size());
	std::string word_indicesHex = getHex(*word_indices, word_indices->size());
	printf("word_indices: \n%s\n", word_indicesHex.data());
	printf("word_indices: \n");
	Poco::UInt64* word_indices_p = (Poco::UInt64*)(word_indices->data());
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
	if (i > 0) printf(" ");
	printf("%4hu", word_indices_p[i]);
	}
	printf("\n");

	printf("\nclear passphrase: \n%s\n", clearPassphrase.data());
	std::string hex_clearPassphrase = getHex((const unsigned char*)clearPassphrase.data(), clearPassphrase.size());
	printf("passphrase bin: \n%s\n\n", hex_clearPassphrase.data());

	//*/

	std::unique_ptr<KeyPairEd25519> key_pair(new KeyPairEd25519);
	if (!key_pair->mSodiumSecret) {
		key_pair->mSodiumSecret = mm->getMemory(crypto_sign_SECRETKEYBYTES);
	}

	crypto_sign_seed_keypair(key_pair->mSodiumPublic, *key_pair->mSodiumSecret, hash);
	
	return std::move(key_pair);

	// print hex for all keys for debugging
	/*	printf("// ********** Keys ************* //\n");
	printf("Sodium Public: \t%s\n", getHex(mSodiumPublic, crypto_sign_PUBLICKEYBYTES).data());
	printf("Sodium Private: \t%s\n", getHex(*mSodiumSecret, mSodiumSecret->size()).data());
	printf("// ********* Keys End ************ //\n");
	*/
	//printf("[KeyPair::generateFromPassphrase] finished!\n");
	// using 
}
MemoryBin* KeyPairEd25519::sign(const unsigned char* message, size_t messageSize) const
{
	
	if (!message || !messageSize) return nullptr;
	if (!mSodiumSecret) return nullptr;
	auto mm = MemoryManager::getInstance();

	const static char functionName[] = "KeyPairEd25519::sign";

	auto signBinBuffer = mm->getMemory(crypto_sign_BYTES);
	unsigned long long actualSignLength = 0;

	// TODO: Add Exceptions
	if (crypto_sign_detached(*signBinBuffer, &actualSignLength, message, messageSize, *mSodiumSecret)) {
		mm->releaseMemory(signBinBuffer);
		throw Ed25519SignException("cannot sign", getPublicKey(), std::string((const char*)message, messageSize));
	}

	if (crypto_sign_verify_detached(*signBinBuffer, message, messageSize, mSodiumPublic) != 0) {
		mm->releaseMemory(signBinBuffer);
		throw Ed25519SignException("sign verify failed", getPublicKey(), std::string((const char*)message, messageSize));
	}

	return signBinBuffer;

}

bool KeyPairEd25519::verify(const std::string& message, const std::string& signature) const
{
	if (message == "" || signature == "") return false;
	if (crypto_sign_verify_detached((const unsigned char*)signature.data(), (const unsigned char*)message.data(), message.size(), mSodiumPublic) != 0) {
		return false;
	}
	return true;
}

MemoryBin* KeyPairEd25519::getPublicKeyCopy() const
{
	auto pubkey = MemoryManager::getInstance()->getMemory(getPublicKeySize());
	pubkey->copyFrom(mSodiumPublic);
	return pubkey;
}

MemoryBin* KeyPairEd25519::getCryptedPrivKey(const std::shared_ptr<SecretKeyCryptography> password) const
{
	if (!password) return nullptr;
	if (!mSodiumSecret) return nullptr;

	MemoryBin* encryptedKey = nullptr;
	if (SecretKeyCryptography::AUTH_ENCRYPT_OK == password->encrypt(mSodiumSecret, &encryptedKey)) {
		return encryptedKey;
	}
	else {
		return nullptr;
	}

}

// ********************** Exceptions *************************************
Ed25519SignException::Ed25519SignException(const char* what, MemoryBin* pubkey, const std::string& message) noexcept
	: GradidoBlockchainException(what), mPubkey(pubkey), mMessage(message)
{

}

Ed25519SignException::Ed25519SignException(const char* what, const unsigned char* pubkey, const std::string& message) noexcept
	: GradidoBlockchainException(what), mMessage(message)
{
	mPubkey = MemoryManager::getInstance()->getMemory(KeyPairEd25519::getPublicKeySize());
	mPubkey->copyFrom(pubkey);
}

Ed25519SignException::~Ed25519SignException()
{
	if (mPubkey) {
		MemoryManager::getInstance()->releaseMemory(mPubkey);
	}
}

std::string Ed25519SignException::getFullString() const
{
	std::string mResult(what());
	mResult += ", with pubkey: " + DataTypeConverter::binToHex(mPubkey);
	mResult += ", with message: " + DataTypeConverter::binToHex(mMessage);
	return mResult;
}