#ifndef SHARED_PTR
#define SHARED_PTR

/*
#include <iostream>

template <typename T>
class SharedPtr
{
private:

    @attr ptr(pointer to T): a pointer to the given position
    @attr count(pointer to int): the number of instances pointing to a same place

    T *ptr;
    int *count;
public:
    // default constructor
    SharedPtr() : ptr(nullptr), count(nullptr) {}

    // constructor
    explicit SharedPtr(T *pointer) : ptr(pointer) 
    {
        if (pointer) count = new int, *count = 1;
        else count = nullptr;
    }

    // copy constructors
    SharedPtr(const SharedPtr &other) : ptr(other.ptr), count(other.count)
    {
        if(this == &other) return;
        if (count) (*count)++;
    }
    
    // operator =
    SharedPtr& operator = (const SharedPtr &other)
    {
        if (this == &other) return *this;
        this -> reset();
        ptr = other.ptr, count = other.count;
        if (count) (*count)++;
        return *this;
    }

    int use_count() { return count ? *count : 0; }
    T* get() const { return ptr; }

    // operator * and operator ->
    T* operator -> () { return ptr; }
    T& operator * () { return *ptr; }

    // operator bool
    operator bool() const { return ptr; }

    // reset()
    void reset()
    {
        if (ptr)
        {
            if (*count > 1)
                (*count)--;
            else
            {
                delete ptr;
                delete count;
            }
            ptr = nullptr, count = nullptr;
        }
    }
    void reset(T* pointer)
    {
        if (ptr == pointer) return;
        reset();
        ptr = pointer;
        if (pointer) count = new int, *count = 1;
    }
    // deconstructor
    ~SharedPtr() { reset(); }
};

template <typename T, typename ... Args>
SharedPtr<T> make_shared(Args&&... args)
{
    return SharedPtr(new T(std :: forward<Args>(args) ...));
}

*/

//Some problems happened when transplant my shared pointer here, so I did some modification.

template<class T>
class SharedPtr {
private:
    T *ptr = nullptr;
    std::size_t *count = nullptr;
public:
    SharedPtr() : ptr(nullptr), count(nullptr) {};

    explicit SharedPtr(T *input) : ptr(input) {
        if (ptr != nullptr) {
            count = new std::size_t;
            *count = 1;
        }
    };

    SharedPtr(SharedPtr const &input) {
        ptr = input.ptr;
        count = input.count;
        if (count != nullptr && this != &input)(*count)++;
    }

    ~SharedPtr() {
        reset();
    }

    operator bool() const {
        if (ptr == nullptr) return false;
        else return true;
    }

    SharedPtr &operator=(SharedPtr const &input) {
        if (this != &input) {
            reset();
            ptr = input.ptr;
            count = input.count;
            if (count != nullptr) (*count)++;
        }
        return *this;
    }

    T *get() const {
        return ptr;
    }

    T &operator*() {
        return *ptr;
    }

    T *operator->() {
        return ptr;
    }

    void reset() {
        if (ptr != nullptr && *count > 1) (*count)--;
        else {
            delete ptr;
            delete count;
        }
        ptr = nullptr;
        count = nullptr;
    }

    void reset(T *input) {
        if (ptr == input) return;
        reset();
        ptr = input;
        if (ptr != nullptr) {
            count = new std::size_t;
            *count = 1;
        }
    }

    std::size_t use_count() const {
        if (ptr == nullptr) return 0;
        else return *count;
    }
};

template<typename T, typename ...Args>
SharedPtr<T> make_shared(Args &&...args) {
    return std::move(SharedPtr<T>{new T(std::forward<Args>(args)...)});
}

#endif