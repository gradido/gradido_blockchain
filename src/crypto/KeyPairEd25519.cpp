
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/crypto/KeyPairEd25519Ex.h"
#include <assert.h>
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "ed25519_bip32_c_interface.h"


KeyPairEd25519::KeyPairEd25519(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr privateKey/* = nullptr*/, memory::ConstBlockPtr chainCode/* = nullptr*/)
	:  mSodiumSecret(privateKey), mChainCode(chainCode), mSodiumPublic(publicKey)
{
	checkKeySizes();
	if (privateKey) {
		auto calculatedPublicKey = calculatePublicKey(*privateKey);
		if (!calculatedPublicKey.isTheSame(*publicKey)) {
			throw ED25519InvalidPrivateKeyForPublicKey("public key don't belong to private key", publicKey->convertToHex());
		}
	}
}


KeyPairEd25519::~KeyPairEd25519()
{
}

std::shared_ptr<KeyPairEd25519> KeyPairEd25519::create(const std::shared_ptr<Passphrase> passphrase)
{
	//auto er = ErrorManager::getInstance();
	assert(passphrase);
	// libsodium doc: https://libsodium.gitbook.io/doc/advanced/hmac-sha2
	// https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki
	
	auto word_indices = passphrase->getWordIndices();
	
	assert(word_indices[0] && word_indices[1] && word_indices[2] && word_indices[3]);
	std::string clear_passphrase = passphrase->createClearPassphrase();

	memory::Block hash(crypto_hash_sha512_BYTES);

	crypto_hash_sha512_state state;
	crypto_hash_sha512_init(&state);

	// ****  convert word indices into uint64  ****
	// To prevent breaking existing passphrase-hash combinations word indices will be put into 64 Bit Variable to mimic first implementation of algorithms
	auto valueSize = sizeof(uint64_t);
	uint64_t value = 0;
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		value = word_indices[i];
		crypto_hash_sha512_update(&state, (const unsigned char*)&value, valueSize);
	}	
	// **** end converting into uint64 *****
	crypto_hash_sha512_update(&state, (unsigned char*)clear_passphrase.data(), clear_passphrase.size());
	crypto_hash_sha512_final(&state, hash);
	normalizeBytesForce3rd(hash);
	/*
	// debug passphrase
	printf("\passsphrase: <%s>\n", passphrase);
	printf("size word indices: %u\n", word_indices->size());
	std::string word_indicesHex = getHex(*word_indices, word_indices->size());
	printf("word_indices: \n%s\n", word_indicesHex.data());
	printf("word_indices: \n");
	uint64_t* word_indices_p = (uint64_t*)(word_indices->data());
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
	if (i > 0) printf(" ");
	printf("%4hu", word_indices_p[i]);
	}
	printf("\n");

	printf("\nclear passphrase: \n%s\n", clearPassphrase.data());
	std::string hex_clearPassphrase = getHex((const unsigned char*)clearPassphrase.data(), clearPassphrase.size());
	printf("passphrase bin: \n%s\n\n", hex_clearPassphrase.data());

	//*/
	auto chainCode = std::make_shared<memory::Block>(ED25519_CHAIN_CODE_SIZE);
	auto publicKey = std::make_shared<memory::Block>(ED25519_PUBLIC_KEY_SIZE);
	auto privateKey = std::make_shared<memory::Block>(ED25519_PRIVATE_KEY_SIZE);
	memcpy(*chainCode, &hash[32], 32);
	crypto_sign_seed_keypair(*publicKey, *privateKey, hash);

	/*
	// print hex for all keys for debugging
	printf("// ********** Keys ************* //\n");
	printf("public: \t%s\n", publicKey->convertToHex().data());
	printf("secret: \t%s\n", privateKey->convertToHex().data());
	printf("chain:  \t%s\n", chainCode->convertToHex().data());
	printf("seed:   \t%s\n", hash.convertToHex().data());
	printf("// ********* Keys End ************ //\n");
	*/
	
	return std::make_shared<KeyPairEd25519>(publicKey, privateKey, chainCode);	
	
	//printf("[KeyPair::generateFromPassphrase] finished!\n");
	// using 
}

memory::Block KeyPairEd25519::calculatePublicKey(const memory::Block& privateKey)
{
	if (privateKey.size() != ED25519_PRIVATE_KEY_SIZE) {
		throw Ed25519InvalidKeyException("invalid key size", privateKey, ED25519_PRIVATE_KEY_SIZE);
	}
	memory::Block pubkey(ED25519_PUBLIC_KEY_SIZE);
	crypto_sign_ed25519_sk_to_pk(pubkey.data(), privateKey);
	return pubkey;
}

Ed25519DerivationType KeyPairEd25519::getDerivationType(uint32_t index)
{
	if (index >= 0x80000000) {
		return Ed25519DerivationType::HARD;
	}
	else {
		return Ed25519DerivationType::SOFT;
	}
}

std::shared_ptr<KeyPairEd25519Ex> KeyPairEd25519::deriveChild(uint32_t index)
{
	if (!mChainCode) {
		throw Ed25519DeriveException("derivation without chain code not possible", mSodiumPublic);
	}
	auto chainCode = std::make_shared<memory::Block>(ED25519_CHAIN_CODE_SIZE);
	
	if (hasPrivateKey()) {
		auto privateKey = std::make_shared<memory::Block>(ED25519_PRIVATE_KEY_SIZE);
		derivePrivateKey(*mSodiumPublic, *mChainCode, index, *privateKey, *chainCode);
		auto publicKey = std::make_shared<memory::Block>(calculatePublicKey(*privateKey));
		return std::make_shared<KeyPairEd25519Ex>(publicKey, privateKey, chainCode, index);
	}
	else {
		auto publicKey = std::make_shared<memory::Block>(ED25519_PUBLIC_KEY_SIZE);
		if (derivePublicKey(*mSodiumSecret, *mChainCode, index, *publicKey, *chainCode)) {
			return std::make_shared<KeyPairEd25519Ex>(publicKey, nullptr, chainCode, index);
		}
		else {
			if (getDerivationType(index) == Ed25519DerivationType::HARD) {
				throw Ed25519DeriveException("hard derivation with public key not possible", mSodiumPublic);
			}
			else {
				throw Ed25519DeriveException("derivePublicKey failed", mSodiumPublic);
			}
		}
	}
}

memory::Block KeyPairEd25519::sign(const unsigned char* message, size_t messageSize) const
{
	assert(message && messageSize);
	if (!mSodiumSecret) {
		throw Ed25519SignException("missing secret key for sign", mSodiumPublic, std::string((const char*)message, messageSize));
	}

	memory::Block signature(ED25519_SIGNATURE_SIZE);
	unsigned long long actualSignLength = 0;

	// TODO: Add Exceptions
	if (crypto_sign_detached(signature, &actualSignLength, message, messageSize, *mSodiumSecret)) {
		throw Ed25519SignException("cannot sign", getPublicKey(), std::string((const char*)message, messageSize));
	}
#ifdef NDEBUG 
	if (crypto_sign_verify_detached(signature, message, messageSize, *mSodiumPublic) != 0) {
		//
		auto calculatedPublicKey = calculatePublicKey(mSodiumSecret);
		if (!calculatedPublicKey.isTheSame(*mSodiumPublic)) {
			throw ED25519InvalidPrivateKeyForPublicKey("sign verify failed, public key don't belong to private key", mSodiumPublic->convertToHex());
		}
		throw Ed25519SignException("sign verify failed", getPublicKey(), std::string((const char*)message, messageSize));
	}
#endif

	return signature;
}

bool KeyPairEd25519::verify(const std::string& message, const std::string& signature) const
{
	if (message == "" || signature == "") return false;
	if (crypto_sign_verify_detached((const unsigned char*)signature.data(), (const unsigned char*)message.data(), message.size(), *mSodiumPublic) != 0) {
		return false;
	}
	return true;
}
bool KeyPairEd25519::verify(const memory::Block& message, const memory::Block& signature) const
{
	if(!message || !signature) return false;
	if(crypto_sign_verify_detached(signature, message, message.size(), *mSodiumPublic) != 0) {
		return false;
	}
	return true;
}

bool KeyPairEd25519::is3rdHighestBitClear() const
{
	if (!mSodiumSecret) {
		throw Ed25519MissingKeyException("missing secret key");
	}
	return 0 == (*mSodiumSecret->data(31) & 0b00100000);
}

memory::Block KeyPairEd25519::getCryptedPrivKey(const std::shared_ptr<SecretKeyCryptography> password) const
{
	assert(password);
	if (!mSodiumSecret) {
		throw Ed25519MissingKeyException("missing secret key");
	}
	return password->encrypt(*mSodiumSecret);
}

void KeyPairEd25519::normalizeBytesForce3rd(memory::Block& key)
{
	assert(key.size() >= 32);
	key[0]  &= 0b11111000;
	key[31] &= 0b00011111;
	key[31] |= 0b01000000;
}

bool KeyPairEd25519::isNormalized(const memory::Block& key)
{
	assert(key.size() >= 32);
	return  (key[0]  & 0b11111000) == key[0] 
		&& ((key[31] & 0b00011111) | 0b01000000) == key[31];
}

void KeyPairEd25519::checkKeySizes()
{
	if (mSodiumSecret && mSodiumSecret->size() != ED25519_PRIVATE_KEY_SIZE) {
		throw Ed25519InvalidKeyException("invalid private key size", *mSodiumSecret, ED25519_PRIVATE_KEY_SIZE);
	}
	if (!mSodiumPublic) {
		throw Ed25519MissingKeyException("missing public key");
	}
	if (mSodiumPublic && mSodiumPublic->size() != ED25519_PUBLIC_KEY_SIZE) {
		throw Ed25519InvalidKeyException("invalid public key size or missing public key", *mSodiumPublic, ED25519_PUBLIC_KEY_SIZE);
	}
	if (mChainCode && mChainCode->size() != ED25519_CHAIN_CODE_SIZE) {
		throw Ed25519InvalidKeyException("invalid chain code size", *mChainCode, ED25519_CHAIN_CODE_SIZE);
	}
}
/*
memory::Block KeyPairEd25519::derivePrivateKey(uint32_t index)
{

}
memory::Block KeyPairEd25519::derivePublicKey(uint32_t index)
{

}
*/
// ********************** Exceptions *************************************
Ed25519SignException::Ed25519SignException(const char* what, memory::ConstBlockPtr pubkey, const std::string& message) noexcept
	: GradidoBlockchainException(what), mPubkey(pubkey), mMessage(message)
{

}

Ed25519SignException::Ed25519SignException(const char* what, const unsigned char* pubkey, const std::string& message) noexcept
	: GradidoBlockchainException(what), mPubkey(std::make_shared<memory::Block>(ED25519_PUBLIC_KEY_SIZE, pubkey)), mMessage(message)
{
}

Ed25519SignException::~Ed25519SignException()
{
}

std::string Ed25519SignException::getFullString() const
{
	std::string mResult(what());
	mResult += ", with pubkey: " + mPubkey->convertToHex();
	mResult += ", with message: " + DataTypeConverter::binToHex(mMessage);
	return mResult;
}

// -----------------------------------------------------------------------------------------
Ed25519VerifyException::Ed25519VerifyException(const char* what, std::string messageString, std::string signatureHex) noexcept
: GradidoBlockchainException(what), mMessageString(messageString), mSignatureHex(signatureHex)
{

}

std::string Ed25519VerifyException::getFullString() const 
{
	std::string result = what();
	if(mMessageString.size()) {
		result += ", message: " + mMessageString;
	}
	if(mSignatureHex.size()) {
		result += ", signature: " + mSignatureHex;
	}
	return std::move(result);
}

rapidjson::Value Ed25519VerifyException::getDetails(rapidjson::Document::AllocatorType& alloc) const 
{ 
	rapidjson::Value result(rapidjson::kObjectType);
	result.AddMember("message", rapidjson::Value(mMessageString.data(), alloc), alloc);
	result.AddMember("signature", rapidjson::Value(mSignatureHex.data(), alloc), alloc);
	return result;
}

// -----------------------------------------------------------------------------------------

Ed25519DeriveException::Ed25519DeriveException(const char* what, memory::ConstBlockPtr pubkey) noexcept
	: GradidoBlockchainException(what), mPubkey(pubkey)
{

}

Ed25519DeriveException::~Ed25519DeriveException()
{
}

std::string Ed25519DeriveException::getFullString() const
{
	std::string mResult(what());
	mResult += ", with pubkey: " + mPubkey->convertToHex();
	return mResult;
}

// ------------------------------------------------------------------------------------------------
Ed25519InvalidKeyException::Ed25519InvalidKeyException(const char* what, const memory::Block& invalidKey, size_t expectedKeySize /* = 0 */) noexcept
: GradidoBlockchainException(what), mKeyHex(invalidKey.convertToHex()), mExpectedKeySize(expectedKeySize)
{

}

Ed25519InvalidKeyException::~Ed25519InvalidKeyException()
{
}

std::string Ed25519InvalidKeyException::getFullString() const
{
	std::string mResult(what());
	if(!mKeyHex.empty()) {
	 	mResult += ", with pubkey: " + mKeyHex;
	}
	if(mExpectedKeySize) {
		mResult += ", expected key size: " + std::to_string(mExpectedKeySize);
	}
	return mResult;
}

// ------------------------------------------------------------------------------------------------

ED25519InvalidPrivateKeyForPublicKey::ED25519InvalidPrivateKeyForPublicKey(const char* what, std::string publicKey) noexcept
	: GradidoBlockchainException(what), mPublicKey(publicKey)
{

}
std::string ED25519InvalidPrivateKeyForPublicKey::getFullString() const
{
	std::string result = what();
	result += ", public key: " + mPublicKey;
	return result;
}