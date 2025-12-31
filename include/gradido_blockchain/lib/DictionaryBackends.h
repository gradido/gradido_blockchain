#ifndef __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_BACKENDS_H
#define __GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_BACKENDS_H

#include <optional>
#include <unordered_map>
#include <deque>
#include <mutex>
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

		bool addDataIndex(DataType data, uint32_t index)
		{
			if (index != mIndexDataLookup.size()) { return false; }
			mIndexDataLookup.push_back(data);
			auto insert2 = mDataIndexLookup.insert({ data, index });
			if (!insert2.second) {
				mIndexDataLookup.pop_back();
				return false;
			}
			return true;
		}

		uint32_t getNextIndex() const {
			return mIndexDataLookup.size();
		}
	protected:
		using MapType = std::unordered_map<DataType, uint32_t, Hash, Equal>;
		// for finding index for specific data
		MapType mDataIndexLookup;
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

		bool addDataIndex(DataType data, uint32_t index)
		{
			std::lock_guard _lock(mWorkingMutex);
			return mMemoryBackend.addDataIndex(data, index);
		}

		uint32_t getNextIndex() const 
		{
			std::lock_guard _lock(mWorkingMutex);
			return mMemoryBackend.getNextIndex();
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

		bool addDataIndex(DataType data, uint32_t index)
		{
			std::unique_lock _lock(mSharedMutex);
			return mMemoryBackend.addDataIndex(data, index);
		}

		uint32_t getNextIndex() const
		{
			std::shared_lock _lock(mSharedMutex);
			return mMemoryBackend.getNextIndex();
		}

	protected:
		Memory<DataType, Hash, Equal> mMemoryBackend;
		mutable std::shared_mutex mSharedMutex;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_LIB_DICTIONARY_BACKENDS_H