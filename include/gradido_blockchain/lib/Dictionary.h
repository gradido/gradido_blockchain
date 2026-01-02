#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H

#include "DictionaryBackends.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/serialization/toJsonString.h"

#include "loguru/loguru.hpp"

#include <unordered_map>
#include <optional>
#include <shared_mutex>
#include <memory>

/*!
* @author Dario Rekowski
* @date 2020-02-06
*
* @brief Create Indices for strings, for more efficient working with bigger data in memory
* So basically every one use only the uint32_t index value for comparisation and stuff and only if the real value is needed,
* they will ask the Dictionary for the real value
*
*/


/*!
 * @brief Bidirectional dictionary mapping data values to unique indices.
 *
 * @tparam DataType
 *   Type of the stored data. Must be hashable and equality comparable.
 *   A specialization of `std::hash<DataType>` is required.
 *   Best to use a `std::shared_ptr` to data object.
 *
 */
template<
	typename DataType,
	typename Hash = std::hash<DataType>,
	typename Equal = std::equal_to<DataType>,
	class Backend = dictionary_backend::Memory<DataType, Hash, Equal>
>
class Dictionary
{
public:
	template<typename... BackendArgs>
	Dictionary(const char* name, BackendArgs&&... args) : mName(name), mBackend(std::forward<BackendArgs>(args)...) {}
	virtual ~Dictionary() {};

	/*!
	 * @brief Clears the dictionary, resetting all data and indices.
	*/
	void reset();

	/*!
	 * @brief Returns the index associated with the given data.
     * @param data The data value to look up.
     * @return The index associated with the data.
     * @throws DictionaryNotFoundException if currently no index for the data exist.
     */
	std::optional<uint32_t> getIndexForData(const DataType& data) const;

	/*!
	 * @brief Checks if the given data exists in the dictionary.
	 * @param data The data value to check.
	 * @return True if the data exists, false otherwise.
	 */
	bool hasData(const DataType& data) const;

	/*!
	 * @brief Returns the data associated with a given index.
	 * @param index The index to look up.
	 * @return The data value associated with the index.
	 * @throws DictionaryNotFoundException if the index does not exist.
	 */
	std::optional<DataType> getDataForIndex(uint32_t index) const;

	/*!
	 * @brief Checks if the given index exists in the dictionary.
	 * @param index The index to check.
	 * @return True if the index exists, false otherwise.
	 */
	bool hasIndex(uint32_t index) const;

	/*!
	 * @brief Gets the index for the given data, or adds a new entry if it does not exist.
	 * @param data The data value to look up or insert.
	 * @return The index associated with the data (new or existing).
	 */
	uint32_t getOrAddIndexForData(DataType data);

	/*!
	 * @brief Adds a new data-index pair to the dictionary.
	 * @param data The data value to insert.
	 * @param index The index to associate with the data.
	 * @return True if the insertion was successful, false if the index already exists.
	 * @throws DictionaryInvalidNewKeyException if the index is not equal to the expected next index.
	 */
	bool addDataIndex(DataType data, uint32_t index);

	/*!
	 * @brief Returns the next index to be assigned for a new entry.
	 * @return The next index value.
	 */
	uint32_t getNextIndex() const;

protected:
	std::string mName;
	Backend mBackend;
};

namespace memory {
	class Block;
	struct ConstBlockPtrHash;
	struct ConstBlockPtrEqual;
	using ConstBlockPtr = std::shared_ptr<const Block>;
}

using PublicKeyDictionary = Dictionary<memory::ConstBlockPtr, memory::ConstBlockPtrHash, memory::ConstBlockPtrEqual>;
using CommunityIdDictionary = Dictionary<std::string>;

class DictionaryException : public GradidoBlockchainException
{
public:
	explicit DictionaryException(const char* what, const char* dictionaryName) noexcept;
protected:
	std::string mDictionaryName;
};

class DictionaryNotFoundException : public DictionaryException
{
public:
	explicit DictionaryNotFoundException(const char* what, const char* dictionaryName, const char* key) noexcept;
	std::string getFullString() const;

protected:
	std::string mKey;
};

class DictionaryInsertException : public DictionaryException
{
public:
	explicit DictionaryInsertException(
		const char* what,
		const char* dictionaryName,
		const char* key,
		const char* data
	) noexcept;

	std::string getFullString() const;
protected:
	std::string mKey;
	std::string mData;
};

class DictionaryInvalidNewKeyException : public DictionaryException
{
public:
	explicit DictionaryInvalidNewKeyException(
		const char* what,
		const char* dictionaryName,
		const char* newKeyString,
		const char* oldKeyString
	) noexcept;

	std::string getFullString() const;

protected:
	std::string mNewKeyString;
	std::string mOldKeyString;
};

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
void Dictionary<DataType, Hash, Equal, Backend>::reset()
{
	mBackend.reset();
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
std::optional<uint32_t> Dictionary<DataType, Hash, Equal, Backend>::getIndexForData(const DataType& data) const
{
	return mBackend.getIndexForData(data);
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
bool Dictionary<DataType, Hash, Equal, Backend>::hasData(const DataType& data) const
{
	return mBackend.getIndexForData(data).has_value();
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
std::optional<DataType> Dictionary<DataType, Hash, Equal, Backend>::getDataForIndex(uint32_t index) const
{
	return mBackend.getDataForIndex(index);
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
bool Dictionary<DataType, Hash, Equal, Backend>::hasIndex(uint32_t index) const
{
	return mBackend.getDataForIndex(index).has_value();
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
uint32_t Dictionary<DataType, Hash, Equal, Backend>::getOrAddIndexForData(DataType data)
{
	auto optionalIndex = mBackend.getIndexForData(data);
	if (!optionalIndex.has_value()) {
		auto index = mBackend.getNextIndex();
		if (mBackend.addDataIndex(data, index)) {
			return index;
		}
		else {
			throw DictionaryInsertException(
				"couldn't add data-index pair",
				mName.c_str(),
				std::to_string(index).c_str(),
				serialization::toJsonString(data).c_str()
			);
		}
	}
	else {
		return optionalIndex.value();
	}
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
bool Dictionary<DataType, Hash, Equal, Backend>::addDataIndex(DataType data, uint32_t index)
{
	if (hasIndex(index)) {
		LOG_F(WARNING, "try to add index which already exist");
		return false;
	}
	if (mBackend.getNextIndex() != index) {
		throw DictionaryInvalidNewKeyException(
			"addValueKeyPair called with invalid new key",
			mName.c_str(),
			std::to_string(index).c_str(),
			std::to_string(mBackend.getNextIndex()).c_str()
		);
	}
	if (mBackend.addDataIndex(data, index)) {
		return true;
	}
	else {
		throw DictionaryInsertException(
			"couldn't add data-index pair2",
			mName.c_str(),
			std::to_string(mBackend.getNextIndex()).c_str(),
			serialization::toJsonString(data).c_str()
		);
	}
}

template<
	typename DataType,
	typename Hash,
	typename Equal,
	class Backend
>
uint32_t Dictionary<DataType, Hash, Equal, Backend>::getNextIndex() const
{
	return mBackend.getNextIndex(); 
}


#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
