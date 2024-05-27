#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_VECTOR_CACHE_ALLOCATOR_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_VECTOR_CACHE_ALLOCATOR_H

#include <map>
#include <mutex>

namespace model {
    namespace protopuf {

        template <class T>
        class VectorCacheAllocator
        {
        public:
            using value_type = T;

            VectorCacheAllocator() noexcept = default;
            ~VectorCacheAllocator() noexcept = default;
            template <class U> VectorCacheAllocator(VectorCacheAllocator<U> const&) noexcept {}

            // Allocate memory block of requested size
            value_type* allocate(size_t size)
            {
                std::lock_guard _lock(mMutex);
                // Check if there's a free block of the requested size
                auto it = mFreeBlocks.find(size);
                if (it != mFreeBlocks.end())
                {
                    value_type* block = it->second;
                    mFreeBlocks.erase(it); // Remove the block from freeBlocks
                    return block;
                }
                else
                {
                    // If no free block available, allocate a new one
                    return static_cast<value_type*>(std::malloc(size * sizeof(value_type)));
                }
            }

            // Deallocate memory block
            void deallocate(value_type* block, size_t size) noexcept
            {
                std::lock_guard _lock(mMutex);
                // Add the block to freeBlocks
                mFreeBlocks.insert({ size, block });
            }

            static size_t getMapSize() { std::lock_guard _lock(mMutex); return mFreeBlocks.size(); }

        private:
            static std::multimap<size_t, value_type*> mFreeBlocks;
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
        std::multimap<std::size_t, T*> VectorCacheAllocator<T>::mFreeBlocks;

        template <typename T>
        std::mutex VectorCacheAllocator<T>::mMutex;
    }

}


#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_VECTOR_CACHE_ALLOCATOR_H