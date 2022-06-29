#include "Poco/Types.h"
#include "Poco/Tuple.h"

#include "gradido_blockchain/crypto/Passphrase.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <sstream>

#define STR_BUFFER_SIZE 25

static std::vector<Poco::Tuple<int, std::string>> g_specialChars = {
	{ 0xa4, "auml" },{ 0x84, "Auml" },
	{ 0xbc, "uuml" },{ 0x9c, "Uuml" },
	{ 0xb6, "ouml" },{ 0x96, "Ouml" },
	{ 0x9f, "szlig" }
};

Passphrase::Passphrase(const std::string& passphrase, const Mnemonic* wordSource)
	: mPassphraseString(filter(passphrase)), mWordSource(wordSource)
{
	memset(mWordIndices, 0, PHRASE_WORD_COUNT * sizeof(Poco::UInt16));
	getWordIndices();
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
					if (c2 == it->get<0>()) {
						auto htmlEntitie = it->get<1>();
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

std::shared_ptr<Passphrase> Passphrase::transform(const Mnemonic* targetWordSource)
{

	if (!targetWordSource || !mWordSource) {
		return nullptr;
	}

	if (createWordIndices()) {
		return create(mWordIndices, targetWordSource);
	}
	return nullptr;
}

std::shared_ptr<Passphrase> Passphrase::create(const std::string& passphrase, const Mnemonic* wordSource)
{
	return std::shared_ptr<Passphrase>(new Passphrase(passphrase, wordSource));
}

std::shared_ptr<Passphrase> Passphrase::create(const MemoryBin* wordIndices, const Mnemonic* wordSource)
{
	if (PHRASE_WORD_COUNT * sizeof(uint16_t) >= wordIndices->size()) {
		return nullptr;
	}

	const uint16_t* word_indices_p = (const uint16_t*)wordIndices->data();
	return create(word_indices_p, wordSource);
}

std::string Passphrase::createClearPassphrase() const
{
	auto word_indices = getWordIndices();
	std::string clear_passphrase;
	auto word_source = &CryptoConfig::g_Mnemonic_WordLists[CryptoConfig::MNEMONIC_BIP0039_SORTED_ORDER];
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		auto word = word_source->getWord(word_indices[i]);
		if (word) {
			clear_passphrase += word;
			clear_passphrase += " ";
		}
	}
	return clear_passphrase;
}

std::shared_ptr<Passphrase> Passphrase::create(const uint16_t wordIndices[PHRASE_WORD_COUNT], const Mnemonic* wordSource)
{
	std::string clearPassphrase;
	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		auto word = wordSource->getWord(wordIndices[i]);
		if (word) {
			clearPassphrase += word;
			clearPassphrase += " ";
		}
		else {
			return nullptr;
		}
	}
	return std::shared_ptr<Passphrase>(new Passphrase(clearPassphrase, wordSource));
}

std::shared_ptr<Passphrase> Passphrase::generate(const Mnemonic* wordSource)
{
	auto mm = MemoryManager::getInstance();
	auto word_indices = mm->getMemory(PHRASE_WORD_COUNT * sizeof(Poco::UInt16));
	uint16_t* word_indices_p = (uint16_t*)word_indices->data();

	for (int i = 0; i < PHRASE_WORD_COUNT; i++) {
		word_indices_p[i] = randombytes_random() % 2048;
	}
	auto result_passphrase = create(word_indices_p, wordSource);
	mm->releaseMemory(word_indices);

	return result_passphrase;
}

bool Passphrase::createWordIndices()
{
	auto mm = MemoryManager::getInstance();
	const char* functionName = "Passphrase::createWordIndices";

	if (!mWordSource) {
		throw PassphraseEmptyWordSourceException();
	}

	//DHASH key = DRMakeStringHash(passphrase);
	size_t pass_phrase_size = mPassphraseString.size();

	char acBuffer[STR_BUFFER_SIZE]; memset(acBuffer, 0, STR_BUFFER_SIZE);
	size_t buffer_cursor = 0;

	// get word indices for hmac key
	unsigned char word_cursor = 0;
	for (auto it = mPassphraseString.begin(); it != mPassphraseString.end(); it++)
	{
		if (*it == ' ') {
			if (buffer_cursor < 3) {
				continue;
			}
			if (PHRASE_WORD_COUNT > word_cursor && mWordSource->isWordExist(acBuffer)) {
				mWordIndices[word_cursor] = mWordSource->getWordIndex(acBuffer);
				//word_indices_old[word_cursor] = word_source->getWordIndex(acBuffer);
			}
			else {
				MnemonicException exception("word don't exist", acBuffer);
				exception.setMnemonic(mWordSource);
				throw exception;
			}
			word_cursor++;
			memset(acBuffer, 0, STR_BUFFER_SIZE);
			buffer_cursor = 0;

		}
		else {
			acBuffer[buffer_cursor++] = *it;
		}
	}
	if (PHRASE_WORD_COUNT > word_cursor && mWordSource->isWordExist(acBuffer)) {
		mWordIndices[word_cursor] = mWordSource->getWordIndex(acBuffer);
		//word_indices_old[word_cursor] = word_source->getWordIndex(acBuffer);
		word_cursor++;
	}

	return true;
}

const Poco::UInt16* Passphrase::getWordIndices()
{
	if (!(mWordIndices[0] | mWordIndices[1] | mWordIndices[2] | mWordIndices[3])) {
		if (createWordIndices()) return mWordIndices;
	}
	return mWordIndices;
}

const Poco::UInt16* Passphrase::getWordIndices() const
{
	return mWordIndices;
}

bool Passphrase::checkIfValid()
{
	if (!mWordSource) return false;
	std::istringstream iss(mPassphraseString);
	std::vector<std::string> results(std::istream_iterator<std::string>{iss},
	std::istream_iterator<std::string>());

	bool existAll = true;
	for (auto it = results.begin(); it != results.end(); it++) {
		if (*it == "\0" || *it == "" || it->size() < 3) continue;
		if (!mWordSource->isWordExist(*it)) {
			return false;
		}
	}
	return true;
}
const Mnemonic* Passphrase::detectMnemonic(const std::string& passphrase, const KeyPairEd25519* keyPair /* = nullptr*/)
{
	std::istringstream iss(passphrase);
	std::vector<std::string> results(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());


	std::string user_public_key_hex;

	if (keyPair) {
		user_public_key_hex = DataTypeConverter::pubkeyToHex(keyPair->getPublicKey());
		//printf("user public key hex: %s\n", user_public_key_hex.data());
	}
    std::string last_words_not_found[CryptoConfig::Mnemonic_Types::MNEMONIC_MAX];
	for (int i = 0; i < CryptoConfig::Mnemonic_Types::MNEMONIC_MAX; i++) {
		Mnemonic& m = CryptoConfig::g_Mnemonic_WordLists[i];
		bool existAll = true;
		int count = 0;
		for (auto it = results.begin(); it != results.end(); it++) {
			if (count >= PHRASE_WORD_COUNT) break;
			if (*it == "\0" || *it == "" || it->size() < 3) continue;
			if (!m.isWordExist(*it)) {
				existAll = false;
				if (i == CryptoConfig::Mnemonic_Types::MNEMONIC_BIP0039_SORTED_ORDER) {
					printf("couldn't find word: %s\n", (*it).data());
				}
				last_words_not_found[i] = (*it);
				// leave inner for-loop
				break;
			}
			count++;
		}
		if (existAll) {
			if (keyPair) {
				std::shared_ptr<Passphrase> test_passphrase(new Passphrase(passphrase, &m));
				test_passphrase->createWordIndices();
				auto key_pair = KeyPairEd25519::create(test_passphrase);
				if (key_pair) {

					if (*key_pair != *keyPair) {
#ifdef _TEST_BUILD // additional infos for debugging if error occure in test
						printf("public key mismatch\n");
						std::string generated_key_pair_hex = DataTypeConverter::pubkeyToHex(key_pair->getPublicKey());
						std::string parameter_key_pair_hex = DataTypeConverter::pubkeyToHex(keyPair->getPublicKey());
						printf("parameter: %s, generated: %s\n", parameter_key_pair_hex.data(), generated_key_pair_hex.data());
#endif
						continue;
					}
				}
			}
			return &CryptoConfig::g_Mnemonic_WordLists[i];
		}
		//printf("last word not found: %s in %s\n", last_words_not_found[i].data(), ServerConfig::mnemonicTypeToString((ServerConfig::Mnemonic_Types)i));
	}
	
	return nullptr;
}
