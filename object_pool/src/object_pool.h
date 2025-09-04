#pragma once

#include <vector>
#include <stack>
#include <memory>


template<typename T>
class ObjectPool;


template<typename T>
class ObjectPtr
{
    T* ptr;
    ObjectPool<T>* pool;

    void reset()
    {
        if (ptr && pool)
        {
            pool->deallocate(ptr);
            ptr = nullptr;
            pool = nullptr;
        }
    }

public:
    ObjectPtr(T* ptr, ObjectPool<T>* pool)
        : ptr(ptr), pool(pool)
    {}

    ~ObjectPtr() { reset(); }

    ObjectPtr(const ObjectPtr&) = delete;

    ObjectPtr& operator=(const ObjectPtr&) = delete;

    ObjectPtr(ObjectPtr&& other) noexcept
        : ptr(other.ptr), pool(other.pool)
    {
        other.ptr = nullptr;
        other.pool = nullptr;
    }

    ObjectPtr& operator=(ObjectPtr&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            ptr = other.ptr;
            pool = other.pool;
            other.ptr = nullptr;
            other.pool = nullptr;
        }
        return *this;
    }

    T* operator->() { return ptr; }

    T& operator*() { return *ptr; }

    explicit operator bool() const { return ptr != nullptr; }
};


template<typename T>
class ObjectPool
{
    std::vector<std::unique_ptr<T[]>> chunks;
    std::stack<T*> free_list;
    size_t chunk_size;

    void allocate_chunk()
    {
        std::unique_ptr<T[]> chunk(new T[chunk_size]);
        T* begin = chunk.get();
        for (size_t i = 0; i < chunk_size; i++) {
            free_list.push(&begin[i]);
        }
        chunks.push_back(std::move(chunk));
    }
public:
    ObjectPool(size_t chunk_size = 16)
        : chunk_size(chunk_size)
    {
        allocate_chunk();
    }

    template <typename... Args>
    ObjectPtr<T> allocate(Args&&... args)
    {
        if (free_list.empty())
            allocate_chunk();
        T* obj = free_list.top();

        try
        {
            new(obj) T(std::forward<Args>(args)...);
            free_list.pop();
        }
        catch(...)
        {
            throw;
        }
        
        return ObjectPtr<T>(obj, this);
    }

    void deallocate(T* ptr)
    {
        if (ptr)
        {
            ptr->~T();
            free_list.push(ptr);
        }
    }

    size_t available() const { return free_list.size(); }
};