#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_VECTOR_CACHE_ALLOCATOR_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_VECTOR_CACHE_ALLOCATOR_H

#include <map>
#include <mutex>
#include <vector>
#include <string>

namespace memory {    

    template <class T>
    class VectorCacheAllocator
    {
    public:
        using value_type = T;

        VectorCacheAllocator() noexcept = default;
        ~VectorCacheAllocator() noexcept = default;
        template <class U> VectorCacheAllocator(VectorCacheAllocator<U> const&) noexcept {}

        // Allocate memory block of requested size
        value_type* allocate(size_t count)
        {   
            auto size = count * sizeof(value_type);
            std::lock_guard _lock(mMutex);
            // Check if there's a free block of the requested size
            auto it = mFreeBlocks.find(size);
            if (it != mFreeBlocks.end())
            {
                T* ptr = static_cast<T*>(it->second);
                mFreeBlocks.erase(it); // Remove the block from freeBlocks
                return ptr;
            }
            else
            {
                // If no free block available, allocate a new one
                return static_cast<value_type*>(std::malloc(size));
            }
        }

        // Deallocate memory block
        void deallocate(value_type* block, size_t count) noexcept
        {
            auto size = count * sizeof(value_type);
            std::lock_guard _lock(mMutex);
            // Add the block to freeBlocks
            mFreeBlocks.insert({ size, block });
        }

        static size_t getMapSize() { std::lock_guard _lock(mMutex); return mFreeBlocks.size(); }

    private:
        static std::multimap<size_t, void*> mFreeBlocks;
        static std::mutex mMutex;
    };

    template <class T, class U>
    bool operator==(VectorCacheAllocator<T> const&, VectorCacheAllocator<U> const&) noexcept
    {
        return true;
    }

    template <class T, class U>
    bool operator!=(VectorCacheAllocator<T> const& x, VectorCacheAllocator<U> const& y) noexcept
    {
        return !(x == y);
    }

    template <typename T>
    std::multimap<std::size_t, void*> VectorCacheAllocator<T>::mFreeBlocks;

    template <typename T>
    std::mutex VectorCacheAllocator<T>::mMutex;

    typedef std::vector<std::byte, VectorCacheAllocator<std::byte>> ByteVectorCachedAlloc;
    typedef std::basic_string<char, std::char_traits<char>, VectorCacheAllocator<char>> StringCachedAlloc;
}


#endif //__GRADIDO_BLOCKCHAIN_MEMORY_VECTOR_CACHE_ALLOCATOR_H