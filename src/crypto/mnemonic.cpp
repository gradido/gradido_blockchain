
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/crypto/MnemonicType.h"
#include "gradido_blockchain/crypto/mnemonic.h"

#include "tinf.h"
#include "magic_enum/magic_enum_utility.hpp"
#include "loguru/loguru.hpp"

#include <memory>
#include <cstring>
#include <assert.h>
#include <mutex>
#include <regex>

using namespace magic_enum;

static std::regex g_checkValidWord("^[a-zA-Z�������&;]*$");

Mnemonic::Mnemonic()
{
	memset(mWords, 0, 2048 * sizeof(char*));
//	mWordHashIndices.resize(2048);
}

Mnemonic::~Mnemonic()
{
	clear();
}



void Mnemonic::init(void(*fill_words_func)(unsigned char*), unsigned int original_size, unsigned int compressed_size)
{
	std::unique_lock<std::shared_mutex> _lock(mWorkingMutex);
	clear();

	unsigned char* buffer = (unsigned char*)malloc(compressed_size);
	unsigned char* uncompressed_buffer = (unsigned char*)malloc(original_size + 1);
	memset(uncompressed_buffer, 0, original_size + 1);
	fill_words_func(buffer);

	// uncompress
	unsigned int original_size_cpy = original_size;

	if (tinf_gzip_uncompress(uncompressed_buffer, &original_size_cpy, buffer, compressed_size) != TINF_OK) {
		free(buffer);
		free(uncompressed_buffer);
		throw MnemonicException("couldn't uncompress");
	}
	if (original_size_cpy != original_size) {
		free(buffer);
		free(uncompressed_buffer);
		throw MnemonicException("uncompressed size don't match");
	}
	else {
		free(buffer);

		//printf("c[Mnemonic::%s] uncompressing success\n", __FUNCTION__);
		// fill words in array and hashList
		std::string uncompressed_file_name = "uncompressed_buffer";
		uncompressed_file_name += std::to_string(original_size);
		uncompressed_file_name += ".txt";
		FILE* f = fopen(uncompressed_file_name.data(), "w");
		if (f) {
			fwrite(uncompressed_buffer, sizeof(char), original_size, f);
			fclose(f);
		}

		unsigned short cursor = 0;
		u32 word_begin = 0, word_end = 0;

		for (unsigned int i = 0; i < original_size; i++) {
			if (cursor >= 2048) {
				throw MnemonicException("more than 2048 words in word list");
			}
			if (uncompressed_buffer[i] == ',' || i == original_size - 1) {
				word_end = i;

				u32 word_size = word_end - word_begin;
				if (word_size < 3) {
					LOG_SCOPE_F(ERROR, "word is smaller than 3");
					LOG_F(ERROR, "word size: %d, index: %d", word_size, cursor);
					char acBuffer[22]; memset(acBuffer, 0, 22);
					int _start = word_end - 10;
					if (_start < 0) {
						_start = 0;
					}
					memcpy(acBuffer, &uncompressed_buffer[_start], 20);
					LOG_F(ERROR, "word_end: %d, word_begin: %d, part: %s", word_end, word_begin, acBuffer);
				}
				if (word_end < word_begin) {
					LOG_SCOPE_F(ERROR, "word_end < word_begin");
					LOG_F(ERROR, "%c %c %c", uncompressed_buffer[i - 1], uncompressed_buffer[i], uncompressed_buffer[i + 1]);
					LOG_F(ERROR, "%s", uncompressed_buffer);
					continue;
				}
				if (uncompressed_buffer[i] != ',') {
					//printf("last char: %c\n", uncompressed_buffer[i]);
					word_size++;
				}
				// + 1 for null terminating
				mWords[cursor] = (char*)malloc(word_size + 1);

				// fill hash list for fast reverse lookup
				memset(mWords[cursor], 0, word_size + 1);
				if (word_begin + word_size > original_size) {
					free(uncompressed_buffer);
					throw MnemonicException("word goes out of array bounds");
				}
				memcpy(mWords[cursor], &uncompressed_buffer[word_begin], word_size);
				//printf("%d: %s\n", cursor, mWords[cursor]);

				DHASH word_hash = DRMakeStringHash(mWords[cursor]);
				//mWordHashIndices.addByHash(word_hash, (void*)cursor);
				auto result = mWordHashIndices.insert({ word_hash, cursor });
				if (!result.second) {
					// handle hash collision
					auto it_collide = mHashCollisionWords.find(word_hash);
					if (it_collide == mHashCollisionWords.end()) {
						std::map<std::string, unsigned short> collidedWordsMap;
						collidedWordsMap.insert({ mWords[result.first->second], result.first->second });
						auto result2 = mHashCollisionWords.insert({ word_hash, collidedWordsMap });
						if (!result2.second) {
							free(uncompressed_buffer);
							throw MnemonicException("couldn't add new entry to mHashCollisionWords");
						}
						it_collide = result2.first;
					}
					assert(it_collide != mHashCollisionWords.end());

					auto result3 = it_collide->second.insert({ mWords[cursor], cursor });
					if (!result3.second) {
						free(uncompressed_buffer);
						throw MnemonicException("couldn't add new word into mHashCollisionWords entry");
					}
				}

				word_begin = i + 1;
				cursor++;
			}
		}
		//printf("c[Mnemonic::%s] before freeing uncompressed buffer \n", __FUNCTION__);
		free(uncompressed_buffer);

		// remove hash colliding entrys from regular map
		for (auto it_collide = mHashCollisionWords.begin(); it_collide != mHashCollisionWords.end(); it_collide++) {
			mWordHashIndices.erase(it_collide->first);
		}
		return;
	}
	//printf("c[Mnemonic::%s] before freeing buffer \n", __FUNCTION__);
	free(buffer);
	throw MnemonicException("reached unexpected end of function");
}

short Mnemonic::getWordIndex(const char* word) const
{
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
	DHASH word_hash = DRMakeStringHash(word);
	auto it = mWordHashIndices.find(word_hash);
	if (it != mWordHashIndices.end()) {
		return it->second;
	}
	auto it_collide = mHashCollisionWords.find(word_hash);
	if (it_collide != mHashCollisionWords.end()) {
		auto it_collided_word = it_collide->second.find(word);
		if (it_collided_word != it_collide->second.end()) {
			return it_collided_word->second;
		}
	}
	return -1;
}

/*
bool Mnemonic::isWordExist(const std::string& word) const
{
	return getWordIndex(word.data()) != -1;
	//DHASH word_hash = DRMakeStringHash(word.data());
	//return mWordHashIndices.find(word_hash) != mWordHashIndices.end();
}
*/

const char* Mnemonic::getWord(short index) const {
	//std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);

	if (index < 2048 && index >= 0) {
		std::string word;
		{
			std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
			word = mWords[index];
		}

		if (!std::regex_match(word, g_checkValidWord)) {
			CryptoConfig::loadMnemonicWordLists();
			{
				std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
				word = mWords[index];
			}
			if (!std::regex_match(word, g_checkValidWord)) {
				throw MnemonicException("empty or invalid word, reload mnemonic word list doesn't helped", word.data());
			}
		}
		{
			std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
			return mWords[index];
		}
	}
	return nullptr;
}

void Mnemonic::clear()
{
	for (int i = 0; i < 2048; i++) {
		if (mWords[i]) {
			free(mWords[i]);
		}
	}
	memset(mWords, 0, 2048 * sizeof(char*));
	mWordHashIndices.clear();
	mHashCollisionWords.clear();
}


std::string Mnemonic::getCompleteWordList()
{
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
	std::string result("");
	//std::string toReplaced[] = { "auml", "ouml", "uuml", "Auml", "Ouml", "Uuml", "szlig" };
	std::regex toReplaced[] = {
		std::regex("&auml;"),
		std::regex("&ouml;"),
		std::regex("&uuml;"),
		std::regex("&Auml;"),
		std::regex("&Ouml;"),
		std::regex("Uuml;"),
		std::regex("&szlig;")
	};
	std::string replaceStrings[] = { "�", "�", "�", "�", "�", "�", "�" };
	for (int i = 0; i < 2048; i++) {
		if (mWords[i]) {
			std::string word = mWords[i];
			for (int s = 0; s < 7; s++) {
				word = std::regex_replace(word, toReplaced[s], replaceStrings[s]);
			}

			result += std::to_string(i) + ": " + word + "\n";
		}
		else {
			result += std::to_string(i) + ": <word empty>\n";
		}
	}
	return result;
}

// comparison, not case sensitive.
bool compare_nocase(const std::string& first, const std::string& second)
{
	unsigned int i = 0;
	while ((i < first.length()) && (i < second.length()))
	{
		if (tolower(first[i]) < tolower(second[i])) return true;
		else if (tolower(first[i]) > tolower(second[i])) return false;
		++i;
	}
	return (first.length() < second.length());
}

std::string Mnemonic::getCompleteWordListSorted()
{
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
	std::string result("");

	std::list<std::string> words;
	for (int i = 0; i < 2048; i++) {
		if (mWords[i]) {
			words.push_back(mWords[i]);
		}
		else {
			printf("missing word on %d\n", i);
		}
	}
	words.sort(compare_nocase);

	//std::string toReplaced[] = { "auml", "ouml", "uuml", "Auml", "Ouml", "Uuml", "szlig" };
	std::regex toReplaced[] = {
		std::regex("&auml;"),
		std::regex("&ouml;"),
		std::regex("&uuml;"),
		std::regex("&Auml;"),
		std::regex("&Ouml;"),
		std::regex("Uuml;"),
		std::regex("&szlig;")
	};
	std::string replaceStrings[] = { "�", "�", "�", "�", "�", "�", "�" };
	int i = 0;
	for(auto it = words.begin(); it != words.end(); it++) {
		std::string word = *it;
		for (int s = 0; s < 7; s++) {
			word = std::regex_replace(word, toReplaced[s], replaceStrings[s]);
		}

		result += std::to_string(i) + ": " + word + "\n";
		i++;
	}
	return result;
}

void Mnemonic::printToFile(const char* filename)
{
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
	FILE* f = fopen(filename, "wt");
	if (f) {
		auto words = getCompleteWordListSorted();
		fwrite(words.data(), 1, words.size(), f);
		fclose(f);
	}
}

std::string Mnemonic::getSortedWordListJsonString()
{
	std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
	std::list<std::string> words;
	for (auto it = mWordHashIndices.begin(); it != mWordHashIndices.end(); it++) {
		words.push_back(mWords[it->second]);
	}
	words.sort();

	std::string json_string;
	json_string += '[';
	for (auto it = words.begin(); it != words.end(); it++) {
		if (it != words.begin()) {
			json_string += ',';
		}
		json_string += *it;
	}
	json_string += ']';
	return json_string;
}


MnemonicException::MnemonicException(const char* what, const char* word/* = nullptr*/) :
	GradidoBlockchainException(what), mMnemonicIndex(-1)
{
	if (word) {
		mWord = word;
	}
}

MnemonicException& MnemonicException::setMnemonic(const Mnemonic* mnemonic)
{
	int mnemonicIndex = 0;
	enum_for_each<MnemonicType>([&mnemonicIndex, mnemonic](auto val) {
		if (mnemonicIndex) return;
		constexpr MnemonicType type = val;
		int i = enum_integer(type);
		if (&CryptoConfig::g_Mnemonic_WordLists[i] == mnemonic) {
			mnemonicIndex = i;
			return;
		}
	});
	mMnemonicIndex = mnemonicIndex;
	return *this;
}

std::string MnemonicException::getFullString() const noexcept
{
	if (mWord.size()) {
		std::string result;
		auto whatString = what();
		result.reserve(strlen(whatString) + mWord.size() + 14);
		result = whatString;
		result += " with word: " + mWord;
		return result;
	}
	else {
		return what();
	}
}
