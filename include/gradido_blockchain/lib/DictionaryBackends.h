#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_BACKENDS_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_BACKENDS_H

#include "DictionaryExceptions.h"

#include <optional>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <limits>
#include <shared_mutex>

namespace dictionary_backend
{
	template<
		typename DataType,
		typename Hash = std::hash<DataType>,
		typename Equal = std::equal_to<DataType>
	>
	class Memory
	{
	public:
		void reset() {
			mDataIndexLookup.clear();
			mIndexDataLookup.clear();
		}

		std::optional<uint32_t> getIndexForData(const DataType& data) const
		{
			auto it = mDataIndexLookup.find(data);
			if (it == mDataIndexLookup.end()) {
				return std::nullopt;
			}
			return it->second;
		}

		std::optional<DataType> getDataForIndex(uint32_t index) const
		{
			if (index >= mIndexDataLookup.size()) {
				return std::nullopt;
			}
			return mIndexDataLookup[index];
		}

		uint32_t getOrAddIndexForData(const DataType& data) 
		{
			if (mIndexDataLookup.size() >= static_cast<size_t>(std::numeric_limits<uint32_t>::max())) {
				throw DictionaryOverflowException("try to add more index data set's as uint32_t as index can handle");
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
	class ThreadsafeMemory
	{
	public:
		void reset() {
			std::lock_guard _lock(mWorkingMutex);
			mMemoryBackend.reset();
		}

		std::optional<uint32_t> getIndexForData(const DataType& data) const
		{
			std::lock_guard _lock(mWorkingMutex);
			return mMemoryBackend.getIndexForData(data);
		}

		std::optional<DataType> getDataForIndex(uint32_t index) const
		{
			std::lock_guard _lock(mWorkingMutex);
			return mMemoryBackend.getDataForIndex(index);
		}

		uint32_t getOrAddIndexForData(const DataType& data)
		{
			std::lock_guard _lock(mWorkingMutex);
			return mMemoryBackend.getOrAddIndexForData(data);
		}

	protected:
		Memory<DataType, Hash, Equal> mMemoryBackend;
		mutable std::mutex mWorkingMutex;
	};


	template<
		typename DataType,
		typename Hash = std::hash<DataType>,
		typename Equal = std::equal_to<DataType>
	>
	class SharedThreadsafeMemory
	{
	public:
		void reset() {
			std::unique_lock _lock(mSharedMutex);
			mMemoryBackend.reset();
		}

		std::optional<uint32_t> getIndexForData(const DataType& data) const
		{
			std::shared_lock _lock(mSharedMutex);
			return mMemoryBackend.getIndexForData(data);
		}

		std::optional<DataType> getDataForIndex(uint32_t index) const
		{
			std::shared_lock _lock(mSharedMutex);
			return mMemoryBackend.getDataForIndex(index);
		}

		uint32_t getOrAddIndexForData(const DataType& data)
		{
			std::unique_lock _lock(mSharedMutex);
			return mMemoryBackend.getOrAddIndexForData(data);
		}

	protected:
		Memory<DataType, Hash, Equal> mMemoryBackend;
		mutable std::shared_mutex mSharedMutex;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_BACKENDS_H