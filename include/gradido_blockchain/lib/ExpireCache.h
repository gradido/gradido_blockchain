#ifndef __GRADIDO_BLOCKCHAIN_LIB_EXPIRE_CACHE_H
#define __GRADIDO_BLOCKCHAIN_LIB_EXPIRE_CACHE_H

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <optional>    

template <class TKey, class TValue>
class ExpireCache
{
public:
    ExpireCache(std::chrono::milliseconds timeout)
        : mTimeout(timeout) {}

    virtual ~ExpireCache() {}

    void add(const TKey& key, const TValue& val)
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::lock_guard lock(mMutex);
        assert(mValues.find(key) == mValues.end());
        mValues[key] = val;
        mValuePairCreationTimes.insert({ now, key });
        removeExpiredValues();
    }

    void remove(const TKey& key)
    {
        std::lock_guard lock(mMutex);
        mValues.erase(key);
        // don't remove from mValuePairCreationTimes to spare us the time for searching for the key
        // it will be removed anyway on timeout
    }

    std::optional<TValue> get(const TKey& key) const
    {
        std::lock_guard lock(mMutex);
        auto it = mValues.find(key);
        if (it != mValues.end()) {
            return it->second;
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

    std::chrono::milliseconds mTimeout;
    std::map<TKey, TValue> mValues;
    std::multimap<std::chrono::time_point<std::chrono::high_resolution_clock>, TKey> mValuePairCreationTimes;
    mutable std::mutex mMutex;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_EXPIRE_CACHE_H