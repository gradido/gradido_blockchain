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
	class Backend = dictionary_backend::Memory<DataType, std::hash<DataType>, std::equal_to<DataType>>
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
     * @return The index associated with the data or nullopt if not exist
     */
	std::optional<uint32_t> getIndexForData(const DataType& data) const;	

	/*!
	 * @brief Returns the data associated with a given index.
	 * @param index The index to look up.
	 * @return The data value associated with the index.
	 * @throws DictionaryNotFoundException if the index does not exist.
	 */
	std::optional<DataType> getDataForIndex(uint32_t index) const;

	/*!
	 * @brief Gets the index for the given data, or adds a new entry if it does not exist.
	 * @param data The data value to look up or insert.
	 * @return The index associated with the data (new or existing).
	 */
	uint32_t getOrAddIndexForData(const DataType& data);

	inline Backend& getBackend() { return mBackend; }

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

using PublicKeyDictionary = 
	Dictionary<memory::ConstBlockPtr, 
	dictionary_backend::Memory<memory::ConstBlockPtr, memory::ConstBlockPtrHash, memory::ConstBlockPtrEqual>
>;
using CommunityIdDictionary = Dictionary<std::string>;

template<typename DataType, class Backend>
void Dictionary<DataType, Backend>::reset()
{
	mBackend.reset();
}

template<typename DataType, class Backend>
std::optional<uint32_t> Dictionary<DataType, Backend>::getIndexForData(const DataType& data) const
{
	return mBackend.getIndexForData(data);
}

template<typename DataType, class Backend>
std::optional<DataType> Dictionary<DataType, Backend>::getDataForIndex(uint32_t index) const
{
	return mBackend.getDataForIndex(index);
}

template<typename DataType, class Backend>
uint32_t Dictionary<DataType, Backend>::getOrAddIndexForData(const DataType& data)
{
	try {
		return mBackend.getOrAddIndexForData(data);
	}
	catch (DictionaryOverflowException& ex) {
		ex.setDictionaryName(mName);
		throw ex;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
