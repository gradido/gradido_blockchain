#ifndef __GRADIDO_LOGIN_SERVER_CRYPTO_PASSPHRASE_H
#define __GRADIDO_LOGIN_SERVER_CRYPTO_PASSPHRASE_H

#include "gradido_blockchain/memory/Block.h"
#include "mnemonic.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <memory>
#include <array>


class KeyPairEd25519;

class GRADIDOBLOCKCHAIN_EXPORT Passphrase
{
public:
	Passphrase() = delete;
	Passphrase(const std::string& passphrase, const Mnemonic* wordSource);

	static std::shared_ptr<Passphrase> create(const std::array<uint16_t, PHRASE_WORD_COUNT>& wordIndices, const Mnemonic* wordSource);
	//! \brief generate new passphrase with random
	static std::shared_ptr<Passphrase> generate(const Mnemonic* wordSource);

	static const Mnemonic* detectMnemonic(const std::string& passphrase, const KeyPairEd25519* userKeyPair = nullptr);

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

	inline const std::array<uint16_t, PHRASE_WORD_COUNT>& getWordIndices() const { return mWordIndices; }

	//! \brief please handle with care! should be only seen by user and admin
	const std::string& getString() const { return mPassphraseString; }

protected:
	void createWordIndices();

	std::string			mPassphraseString;
	const Mnemonic*		mWordSource;
	std::array<uint16_t, PHRASE_WORD_COUNT> mWordIndices;
};

class PassphraseEmptyWordSourceException : public GradidoBlockchainException
{
public:
	explicit PassphraseEmptyWordSourceException() : GradidoBlockchainException("empty") {};
	std::string getFullString() const { return what(); }
};

#endif // __GRADIDO_LOGIN_SERVER_CRYPTO_PASSPHRASE

