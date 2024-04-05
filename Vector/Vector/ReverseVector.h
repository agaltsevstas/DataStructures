#ifndef ReverseVector_h
#define ReverseVector_h

template<class T>
class ReverseIterator
{
public:
    explicit ReverseIterator(T* ptr): _ptr(ptr) {}
    operator T*()
    {
        return _ptr;
    }
    
    ReverseIterator& operator++()
    {
        --_ptr;
        return *this;
    }
    
    ReverseIterator operator++(int)
    {
        ReverseIterator tmp(_ptr);
        --_ptr;
        return tmp;
    }
    
    ReverseIterator& operator--()
    {
        ++_ptr;
        return *this;
    }
    
    ReverseIterator operator--(int)
    {
        ReverseIterator tmp(_ptr);
        ++_ptr;
        return tmp;
    }
    
    T operator*() noexcept
    {
        return *_ptr;
    }
    
    bool operator!=(ReverseIterator& other) const
    {
        return _ptr != other._ptr;
    }
private:
    T* _ptr;
};

template <class T>
auto Begin(const T& vector) -> decltype(vector.Begin())
{
    return vector.Begin();
}

template <class T>
auto End(const T& vector) -> decltype(vector.End())
{
    return vector.End();
}

#endif /* ReverseVector_h */
