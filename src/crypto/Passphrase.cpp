#include "gradido_blockchain/crypto/Passphrase.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/MnemonicType.h"
#ifdef _DEBUG
#include "gradido_blockchain/crypto/CryptoConfig.h"
#endif
#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum_utility.hpp"

#include <sstream>

#define STR_BUFFER_SIZE 25

using namespace magic_enum;

static std::vector<std::pair<int, std::string>> g_specialChars = {
	{ 0xa4, "auml" },{ 0x84, "Auml" },
	{ 0xbc, "uuml" },{ 0x9c, "Uuml" },
	{ 0xb6, "ouml" },{ 0x96, "Ouml" },
	{ 0x9f, "szlig" }
};

Passphrase::Passphrase(const std::string& passphrase, MnemonicType wordListType)
	: mPassphraseString(filter(passphrase)), mWordListType(wordListType)
{
	mWordIndices.fill(0);
	createWordIndices();
}


std::string Passphrase::filter(const std::string& passphrase)
{
	std::string filteredPassphrase;
	auto passphrase_size = passphrase.size();

	for (int i = 0; i < passphrase_size; i++) {
		unsigned char c = passphrase.data()[i];
		// asci 128 even by utf8 (hex)
		// 0000 0000 – 0000 007F
		// utf8
		if (c > 0x0000007F) {
			int additionalUtfByteCount = 0;
			//filteredPassphrase += c;
			if ((c & 0x00000080) == 0x00000080) {
				// c3 a4 => ä
				// c3 bc => ü
				// c3 b6 => ö
				// c3 84 => Ä
				// c3 96 => Ö
				// c3 9c => Ü
				// c3 9f => ß



				unsigned char c2 = passphrase.data()[i + 1];
				bool insertedHtmlEntitie = false;
				for (auto it = g_specialChars.begin(); it != g_specialChars.end(); it++) {
					if (c2 == it->first) {
						auto htmlEntitie = it->second;
						filteredPassphrase += "&";
						filteredPassphrase += htmlEntitie;
						filteredPassphrase += ";";
						i++;
						insertedHtmlEntitie = true;
						break;
					}
				}
				if (insertedHtmlEntitie) continue;
				additionalUtfByteCount = 1;
			}
			else if ((c & 0x00000800) == 0x00000800) {
				additionalUtfByteCount = 2;
			}
			else if ((c & 0x00010000) == 0x00010000) {
				additionalUtfByteCount = 3;
			}
			for (int j = 0; j <= additionalUtfByteCount; j++) {
				filteredPassphrase += passphrase.data()[i + j];
				i++;
			}
		}
		else {
			// 32 = Space
			// 65 = A
			// 90 = Z
			// 97 = a
			// 122 = z
			// 59 = ;
			// 38 = &
			if (c == 32 || c == 59 || c == 38 ||
				(c >= 65 && c <= 90) ||
				(c >= 97 && c <= 122)) {
				filteredPassphrase += c;
			}
			else if (c == '\n' || c == '\r') {
				filteredPassphrase += ' ';
			}
		}

	}
	return filteredPassphrase;
}

std::shared_ptr<Passphrase> Passphrase::transform(MnemonicType wordListType)
{
	return create(mWordIndices, wordListType);
}

std::string Passphrase::createClearPassphrase() const
{
	auto word_indices = getWordIndices();
	std::string clear_passphrase;
	auto word_source = CryptoConfig::getWordList(MnemonicType::BIP0039_SORTED_ORDER);
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		auto word = word_source->getWord(word_indices[i]);
		if (word) {
			clear_passphrase += word;
			clear_passphrase += " ";
		}
	}
	return clear_passphrase;
}

std::shared_ptr<Passphrase> Passphrase::create(const std::array<uint16_t, PHRASE_WORD_COUNT>& wordIndices, MnemonicType wordListType)
{
	std::string clearPassphrase;
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		auto word = CryptoConfig::getWordList(wordListType)->getWord(wordIndices[i]);
		if (word) {
			clearPassphrase += word;
			clearPassphrase += " ";
		}
		else {
			return nullptr;
		}
	}
	return std::shared_ptr<Passphrase>(new Passphrase(clearPassphrase, wordListType));
}

std::shared_ptr<Passphrase> Passphrase::generate(MnemonicType wordListType)
{
	std::array<uint16_t, PHRASE_WORD_COUNT> word_indices;
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		word_indices[i] = randombytes_random() % 2048;
	}
	return create(word_indices, wordListType);
}

void Passphrase::createWordIndices()
{
	size_t passphraseSize = mPassphraseString.size();

	std::array<char, STR_BUFFER_SIZE> buffer;
	buffer.fill(0);
	size_t bufferCursor = 0;
	auto wordList = CryptoConfig::getWordList(mWordListType);

	// get word indices for hmac key
	unsigned char word_cursor = 0;
	for (auto it = mPassphraseString.begin(); it != mPassphraseString.end(); it++)
	{
		if (*it == ' ') {
			if (bufferCursor < 3) {
				continue;
			}
			if (PHRASE_WORD_COUNT > word_cursor && wordList->isWordExist(buffer.data())) {
				mWordIndices[word_cursor] = wordList->getWordIndex(buffer.data());
			}
			else {
				MnemonicException exception("word don't exist", buffer.data());
				exception.setWordListType(mWordListType);
				throw exception;
			}
			word_cursor++;
			buffer.fill(0);
			bufferCursor = 0;

		}
		else {
			assert(bufferCursor < buffer.size());
			buffer[bufferCursor++] = *it;
		}
	}
	if (PHRASE_WORD_COUNT > word_cursor && wordList->isWordExist(buffer.data())) {
		mWordIndices[word_cursor] = wordList->getWordIndex(buffer.data());
	}
}

bool Passphrase::checkIfValid()
{
	std::istringstream iss(mPassphraseString);
	std::vector<std::string> results(
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>()
	);

	bool existAll = true;
	for (auto it = results.begin(); it != results.end(); it++) {
		if (*it == "\0" || *it == "" || it->size() < 3) continue;
		if (!CryptoConfig::getWordList(mWordListType)->isWordExist(it->data())) {
			return false;
		}
	}
	return true;
}
MnemonicType Passphrase::detectMnemonic(const std::string& passphrase, const KeyPairEd25519* userKeyPair /* = nullptr*/)
{
	std::istringstream iss(passphrase);
	std::vector<std::string> results(
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>()
	);

#ifdef _DEBUG
	std::string userPublicKeyHex;
	if (userKeyPair)
	{
	    LOG_SCOPE_F(9, __FUNCTION__);
		userPublicKeyHex = userKeyPair->getPublicKey()->convertToHex();
		LOG_F(9, "user public key hex: %s\n", userPublicKeyHex.data());
	}
#endif
    std::string last_words_not_found[enum_integer(MnemonicType::MAX)];
	MnemonicType result = MnemonicType::MAX;
	enum_for_each<MnemonicType>([&](auto val) {
		if (result != MnemonicType::MAX)  return;
		constexpr MnemonicType type = val;
		if(type == MnemonicType::MAX) return;
		int i = enum_integer(type);
		Mnemonic& m = CryptoConfig::g_Mnemonic_WordLists[i];
		bool existAll = true;
		int count = 0;
		for (auto it = results.begin(); it != results.end(); it++) {
			if (count >= PHRASE_WORD_COUNT) break;
			if (*it == "\0" || *it == "" || it->size() < 3) continue;
			if (!m.isWordExist(it->data())) {
				existAll = false;
				if (i == enum_integer(MnemonicType::BIP0039_SORTED_ORDER)) {
					printf("couldn't find word: %s\n", (*it).data());
				}
				last_words_not_found[i] = (*it);
				// leave inner for-loop
				break;
			}
			count++;
		}
		if (existAll)
		{
			if (userKeyPair)
			{
				std::shared_ptr<Passphrase> test_passphrase(new Passphrase(passphrase, type));
				test_passphrase->createWordIndices();
				auto keyPairFromPassphrase = KeyPairEd25519::create(test_passphrase);
				if (keyPairFromPassphrase)
				{
					if (!keyPairFromPassphrase->isTheSame(*userKeyPair))
					{
#ifdef _DEBUG  // additional infos for debugging if error occure in test
                        LOG_SCOPE_F(9, __FUNCTION__);
						LOG_F(9, "public key mismatch");
						LOG_F(9, "user public key: %s, public key from passphrase: %s\n",
							userPublicKeyHex.data(),
							keyPairFromPassphrase->getPublicKey()->convertToHex().data()
						);
#endif
						// continue;
						return;
					}
				}
			}
			result = type;
			// return &CryptoConfig::g_Mnemonic_WordLists[i];
		}
#ifdef _DEBUG
		LOG_F(9, "last word not found: %s in %s",
			last_words_not_found[i].data(),
			enum_name(type).data()
		);
#endif
	});
	return result;
}
