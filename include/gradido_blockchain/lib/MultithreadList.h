#ifndef _GRADIDO_BLOCKCHAIN_LIB_MULTITHREAD_LIST_H__
#define _GRADIDO_BLOCKCHAIN_LIB_MULTITHREAD_LIST_H__

#include "MultithreadContainer.h"
#include <list>
#include <shared_mutex>

/*!

 \brief Container class for mutex protected list

 \author Dario Rekowski

 \date 02.07.2024
*/
template <class ResourceType>
class MultithreadList : protected std::list<ResourceType> {
public:
    virtual ~MultithreadList() {
        clear();
    }

    void clear() {
        std::unique_lock _lock(mSharedMutex);
        std::list<ResourceType>::clear();
    }

    void push_back(ResourceType val) {
        std::unique_lock _lock(mSharedMutex);
        std::list<ResourceType>::push_back(val);
    }

    void push_front(ResourceType val) {
        std::unique_lock _lock(mSharedMutex);
        std::list<ResourceType>::push_front(val);
    }


    bool empty() const {
        std::shared_lock _lock(mSharedMutex);
        return std::list<ResourceType>::empty();
    }

    //! \return false if no values are there
    //! \return true if value is there, gave val a copy from the value at the front of list
    bool pop_front(ResourceType& val) {
        std::unique_lock _lock(mSharedMutex);
        if (!std::list<ResourceType>::empty()) {
            val = std::list<ResourceType>::front();
            std::list<ResourceType>::pop_front();
            return true;
        }
        return false;
    }

    //! \return false if no values are there
    //! \return true if value is there, gave val a copy from the value at the front of list
    bool pop_back(ResourceType& val) {
        std::unique_lock _lock(mSharedMutex);
        if (!std::list<ResourceType>::empty()) {
            val = std::list<ResourceType>::back();
            std::list<ResourceType>::pop_back();
            return true;
        }
        return false;
    }

    inline size_t size() const {
        lock();
        size_t _size = std::list<ResourceType>::size();
        unlock();
        return _size;
    }

    // Extended functionality: allow iterating over the elements
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ResourceType;
        using difference_type = std::ptrdiff_t;
        using pointer = ResourceType*;
        using reference = ResourceType&;

        iterator(ResourceType*) : lock_(mSharedMutex), current_(p) {}
        reference operator*() const { return *current_; }
        pointer operator->() const { return current_; }
        iterator& operator++() {
            ++current_;
            return *this;
        }
        bool operator==(const iterator& other) const { return current_ == other.current_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        std::shared_lock<std::shared_mutex> lock_;  // Acquire read-only lock in constructor
        ResourceType* current_;
    };

    iterator begin() { return iterator(&list_.begin());}
    const iterator begin() const { return iterator(&list_.begin()); }
    iterator end() { return iterator(&list_.end()); }
    const iterator end() const { return iterator(&list_.end()); }

    friend class iterator;
    using iterator = iterator;

protected:
    mutable std::shared_mutex mSharedMutex;
};

#endif // _GRADIDO_BLOCKCHAIN_LIB_MULTITHREAD_LIST_H__