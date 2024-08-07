#ifndef DR_MNEMONIC_H
#define DR_MNEMONIC_H

/*!
 * 
 * @author: einhornimmond
 * 
 * @date: 16.06.19
 * 
 * @desc: Class for handling mnemonic word list, unpacking, reverse lookup
 * 
 */
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/DRHashList.h"
#include <string>
#include <shared_mutex>
#include <map>
#include <list>
#include <vector>

#define PHRASE_WORD_COUNT 24


class GRADIDOBLOCKCHAIN_EXPORT Mnemonic
{
public: 
	Mnemonic();
	~Mnemonic();

	void init(void(*fill_words_func)(unsigned char*), unsigned int original_size, unsigned int compressed_size);

	const char* getWord(short index) const;
	short getWordIndex(const char* word) const;
	inline bool isWordExist(const char* word) const {
		std::shared_lock<std::shared_mutex> _lock(mWorkingMutex);
		return getWordIndex(word) != -1; 
	}
	// using only for debugging
	std::string getCompleteWordList();

	std::string getCompleteWordListSorted();

	std::string getSortedWordListJsonString();

	void printToFile(const char* filename);

protected:

	void clear();

	struct HashCollisionWords {
		DHASH hash;
		std::vector<std::string> words;
	};

	char* mWords[2048];
	std::map<DHASH, unsigned short> mWordHashIndices;
	std::map<DHASH, std::map<std::string, unsigned short>> mHashCollisionWords;
	mutable std::shared_mutex mWorkingMutex;
};

class MnemonicException : public GradidoBlockchainException
{
public:
	explicit MnemonicException(const char* what, const char* word = nullptr);
	std::string getFullString() const noexcept;

	MnemonicException& setMnemonic(const Mnemonic* mnemonic);
	inline int getMnemonicIndex() { return mMnemonicIndex; }

protected:
	std::string mWord;
	int mMnemonicIndex;
};



#endif //DR_MNEMONIC_H
