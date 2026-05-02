#include <utility>

template <typename T>
class UniquePtr {

public:
    UniquePtr(T* resource = nullptr)
        : mPtr { resource }
    {
    }

    ~UniquePtr()
    {
        delete mPtr; // Ok to delete nullptr
    }

    // Delete copy constructor
    UniquePtr(const UniquePtr<T>& other) = delete;

    // Move Constructor
    UniquePtr(UniquePtr<T>&& other)
        : mPtr { other.release() }
    {
    }

    // Converting constructor
    template <typename U>
    UniquePtr(UniquePtr<U>&& other)
        : mPtr { other.release() }
    {
    }

    // Delete copy assignment
    UniquePtr& operator=(const UniquePtr<T>& other) = delete;

    // Move assignment
    UniquePtr& operator=(UniquePtr<T>&& other)
    {
        swap(other);
        return *this;
    }

    // Dereference and arrow operators
    T& operator*() const { return *mPtr; }
    T* operator->() const { return mPtr; }

    T* get() const { return mPtr; }

    // Comparision
    bool operator==(const UniquePtr<T>& other) const { return mPtr == other.mPtr; }

    // Release ownership of the raw pointer and return it
    T* release()
    {
        T* tmp = mPtr;
        mPtr = nullptr;
        return tmp;
    }

    void reset(T* newPtr = nullptr)
    {
        delete mPtr;
        mPtr = newPtr;
    }

    void swap(UniquePtr<T>& other) { std::swap(mPtr, other.mPtr); }

    operator bool() const { return mPtr != nullptr; }

private:
    T* mPtr;
};

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args)
{
    T* rawPtr = new T { std::forward<Args>(args)... };
    return UniquePtr { rawPtr };
}
