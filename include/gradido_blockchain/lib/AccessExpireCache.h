#ifndef __GRADIDO_BLOCKCHAIN_LIB_ACCESS_EXPIRE_CACHE_H
#define __GRADIDO_BLOCKCHAIN_LIB_ACCESS_EXPIRE_CACHE_H

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

template <class TKey, class TValue>
class AccessExpireCache
{
public:
    AccessExpireCache(std::chrono::milliseconds timeout)
        : mTimeout(timeout) {}

    virtual ~AccessExpireCache() {}

    void add(const TKey& key, const TValue& val)
    {
        std::lock_guard lock(mMutex);
        auto it = mValues.insert({ key ,val });
        assert(it.second);
        mValuePairCreationTimes.insert({ it.first->second.timepoint, key });
        removeExpiredValues();
    }

    void remove(const TKey& key)
    {
        std::lock_guard lock(mMutex);
        mValues.erase(key);
        // don't remove from mValuePairCreationTimes to spare us the time for searching for the key
        // it will be removed anyway on timeout
    }

    std::optional<TValue> get(const TKey& key)
    {
        std::lock_guard lock(mMutex);
        auto it = mValues.find(key);
        if (it != mValues.end()) {
            // update timepoint and position in timepoint multimap
            auto outOfRangeTimepointIt = mValuePairCreationTimes.upper_bound(it->second.timepoint);
            for (auto timePointsIt = mValuePairCreationTimes.lower_bound(it->second.timepoint); timePointsIt != outOfRangeTimepointIt; timePointsIt++) {
                if (timePointsIt->second == key) {
                    mValuePairCreationTimes.erase(timePointsIt);
                    it->second.timepoint = std::chrono::high_resolution_clock::now();
                    mValuePairCreationTimes.insert({ it->second.timepoint, key });
                    break;
                }
            }
            return it->second.value;
        }
        return std::nullopt;
    }

    void clear()
    {
        std::lock_guard lock(mMutex);
        mValues.clear();
        mValuePairCreationTimes.clear();
    }

    std::size_t size()
    {
        std::lock_guard lock(mMutex);
        return mValues.size();
    }


protected:
    void removeExpiredValues()
    {
        auto now = std::chrono::high_resolution_clock::now();
        for (auto it = mValuePairCreationTimes.begin(); it != mValuePairCreationTimes.end();)
        {
            if (now - it->first > mTimeout)
            {
                mValues.erase(it->second);
                it = mValuePairCreationTimes.erase(it);
            }
            else {
                break;
            }
        }
    }

    //template<class TValue>
    struct ValueTimePoint
    {
//        template<class TValue>
        ValueTimePoint(TValue _value)
            : value(_value), timepoint(std::chrono::high_resolution_clock::now())
        {
        }
        TValue value;
        std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
    };

    std::chrono::milliseconds mTimeout;
    std::map<TKey, ValueTimePoint> mValues;
    std::multimap<std::chrono::time_point<std::chrono::high_resolution_clock>, TKey> mValuePairCreationTimes;
    mutable std::mutex mMutex;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_ACCESS_EXPIRE_CACHE_H