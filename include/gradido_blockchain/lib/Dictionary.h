#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H

#include "DictionaryExceptions.h"
#include "DictionaryInterface.h"

#include <unordered_map>
#include <optional>
#include <deque>
#include <shared_mutex>
#include <memory>


template<
	typename DataType,
	typename Hash = std::hash<DataType>,
	typename Equal = std::equal_to<DataType>
>
class RuntimeDictionary: public IMutableDictionary<DataType>
{
public:
	RuntimeDictionary(std::string_view name) :mName(name) {}

	virtual void reset() override {
		mDataIndexLookup.clear();
		mIndexDataLookup.clear();
	}

	virtual std::optional<uint32_t> getIndexForData(const DataType& data) const override
	{
		auto it = mDataIndexLookup.find(data);
		if (it == mDataIndexLookup.end()) {
			return std::nullopt;
		}
		return it->second;
	}

	virtual std::optional<DataType> getDataForIndex(uint32_t index) const override
	{
		if (index >= mIndexDataLookup.size()) {
			return std::nullopt;
		}
		return mIndexDataLookup[index];
	}

	virtual uint32_t getOrAddIndexForData(const DataType& data) override
	{
		// TODO: write unit test to test boundaries
		if (mIndexDataLookup.size() > static_cast<size_t>(std::numeric_limits<uint32_t>::max())) {
			throw DictionaryOverflowException("try to add more index data set's as uint32_t as index can handle", mName);
		}
		uint32_t index = static_cast<uint32_t>(mIndexDataLookup.size());
		auto insertIt = mDataIndexLookup.insert({ data, index });
		if (!insertIt.second) {
			return insertIt.first->second;
		}
		mIndexDataLookup.push_back(data);
		return index;
	}

protected:
	std::string mName;
	// for finding index for specific data
	std::unordered_map<DataType, uint32_t, Hash, Equal> mDataIndexLookup;
	// for finding data for a specific index
	std::deque<DataType> mIndexDataLookup;
};

template<
	typename DataType,
	typename Hash = std::hash<DataType>,
	typename Equal = std::equal_to<DataType>
>
class ThreadsafeRuntimeDictionary : public RuntimeDictionary<DataType, Hash, Equal>
{
public:
	ThreadsafeRuntimeDictionary(std::string_view name) : RuntimeDictionary<DataType, Hash, Equal>(name) {}

	virtual void reset() override {
		std::unique_lock _lock(mSharedMutex);
		RuntimeDictionary<DataType, Hash, Equal>::reset();
	}

	virtual std::optional<uint32_t> getIndexForData(const DataType& data) const override
	{
		std::shared_lock _lock(mSharedMutex);
		return RuntimeDictionary<DataType, Hash, Equal>::getIndexForData(data);
	}

	virtual std::optional<const DataType&> getDataForIndex(uint32_t index) const override
	{
		std::shared_lock _lock(mSharedMutex);
		return RuntimeDictionary<DataType, Hash, Equal>::getDataForIndex(index);
	}

	virtual uint32_t getOrAddIndexForData(const DataType& data) override
	{
		std::unique_lock _lock(mSharedMutex);
		return RuntimeDictionary<DataType, Hash, Equal>::getOrAddIndexForData(data);
	}

protected:
	std::shared_mutex mSharedMutex;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_H
