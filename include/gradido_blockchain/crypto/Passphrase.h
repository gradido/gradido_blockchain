#ifndef __GRADIDO_LOGIN_SERVER_CRYPTO_PASSPHRASE_H
#define __GRADIDO_LOGIN_SERVER_CRYPTO_PASSPHRASE_H

//#include <string>
#include "mnemonic.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include <memory>


class KeyPairEd25519;

class Passphrase
{
public:
	Passphrase(const std::string& passphrase, const Mnemonic* wordSource);

	static std::shared_ptr<Passphrase> create(const uint16_t wordIndices[PHRASE_WORD_COUNT], const Mnemonic* wordSource);
	static std::shared_ptr<Passphrase> create(const MemoryBin* wordIndices, const Mnemonic* wordSource);
	static std::shared_ptr<Passphrase> create(const std::string& passphrase, const Mnemonic* wordSource);
	//! \brief generate new passphrase with random
	static std::shared_ptr<Passphrase> generate(const Mnemonic* wordSource);
	static const Mnemonic* detectMnemonic(const std::string& passphrase, const KeyPairEd25519* keyPair = nullptr);

	//! \brief transform passphrase into another language/mnemonic source
	//! \return this if targetWordSource is the same as mWordSource
	std::shared_ptr<Passphrase> transform(const Mnemonic* targetWordSource);

	//! \brief create clear passphrase from word indices from bitcoin word list (bip0039)
	//! 
	//! Used by hashing function to get really the same string,
	//! even user has typed in some not filtered character
	std::string createClearPassphrase() const;

	//! \brief replace utf8 characters with html special character encoding
	//! 
	//! TODO: add more utf8 chars for other languages as they needed
	static std::string filter(const std::string& passphrase);
	
	//! \return true if all words in passphrase existing in mWordSource
	bool checkIfValid();

	const uint16_t* getWordIndices();
	const uint16_t* getWordIndices() const;

	//! \return true if ok
	bool createWordIndices();

	//! \brief please handle with care! should be only seen by user and admin
	const std::string& getString() const { return mPassphraseString; }

protected:
	
	
	std::string			mPassphraseString;
	const Mnemonic*		mWordSource;
	uint16_t		mWordIndices[PHRASE_WORD_COUNT];
};

class PassphraseEmptyWordSourceException : public GradidoBlockchainException
{
public:
	explicit PassphraseEmptyWordSourceException() : GradidoBlockchainException("empty") {};
	std::string getFullString() const { return what(); }
};

#endif // __GRADIDO_LOGIN_SERVER_CRYPTO_PASSPHRASE

