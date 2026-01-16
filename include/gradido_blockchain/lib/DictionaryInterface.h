#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_INTERFACE_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_INTERFACE_H

#include <optional>
#include "gradido_blockchain/types.h"

/*!
* @author einhornimmond
* @date 2020-02-06
* @date 2025-12-31 refactor
*
* @brief Create Indices for strings, for more efficient working with bigger data in memory
* So basically every one use only the uint32_t index value for comparisation and stuff and only if the real value is needed,
* they will ask the Dictionary for the real value*
*/

template<typename DataType>
class IDictionary
{
public:
	virtual ~IDictionary() = default;

	/*!
	 * @brief Returns the index associated with the given data.
	 * @param data The data value to look up.
	 * @return The index associated with the data or nullopt if not exist
	 */
	virtual std::optional<uint32_t> getIndexForData(const DataType& data) const = 0;
	/*!
	 * TODO: remove usage
	 * @brief Returns the data associated with a given index.
	 * @param index The index to look up.
	 * @return The data value associated with the index.
	 * @throws DictionaryNotFoundException if the index does not exist.
	 */
	virtual std::optional<DataType> getDataForIndex(uint32_t index) const = 0;
};


template<typename DataType>
class IMutableDictionary : public IDictionary<DataType>
{
public:
	/*!
	 * @brief Clears the dictionary, resetting all data and indices.
	*/
	virtual void reset() = 0;
	/*!
	 * @brief Gets the index for the given data, or adds a new entry if it does not exist.
	 * @param data The data value to look up or insert.
	 * @return The index associated with the data (new or existing).
	 */
	virtual uint32_t getOrAddIndexForData(const DataType& data) = 0;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_INTERFACE_H
