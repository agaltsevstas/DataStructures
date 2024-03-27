#ifndef Header_h
#define Header_h

#include <iostream>


template<class T>
class ReverseIterator
{
public:
    explicit ReverseIterator(T* ptr) noexcept: _ptr(ptr) {}
    operator T*()
    {
        return _ptr;
    }
    
    ReverseIterator& operator++() noexcept
    {
        --_ptr;
        return *this;
    }
    
    ReverseIterator operator++(int) noexcept
    {
        ReverseIterator tmp(_ptr);
        --_ptr;
        return tmp;
    }
    
    ReverseIterator& operator--() noexcept
    {
        ++_ptr;
        return *this;
    }
    
    ReverseIterator operator--(int) noexcept
    {
        ReverseIterator tmp(_ptr);
        ++_ptr;
        return tmp;
    }
    
    T operator*() noexcept
    {
        return *_ptr;
    }
    
    bool operator!=(ReverseIterator& other) const noexcept
    {
        return _ptr != other._ptr;
    }
private:
    T* _ptr;
};

template <class T>
auto Begin(const T& array) -> decltype(array.Begin())
{
    return array.Begin();
}

template <class T>
auto End(const T& array) -> decltype(array.End())
{
    return array.End();
}


template <class T, size_t N>
class Array
{
    using size_type = size_t;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = ReverseIterator<T>;
    using const_reverse_iterator = ReverseIterator<T>;
    
public:
    Array() = default;
    ~Array() = default;
    constexpr Array(const std::initializer_list<T>& array);
    constexpr Array(const Array& other) noexcept;
    constexpr Array(Array&& other) noexcept;
    constexpr Array& operator=(const Array& other) noexcept;
    constexpr Array& operator=(Array&& other) noexcept;
    constexpr bool operator==(const Array& other) const;
    constexpr bool operator!=(const Array& other) const;
    constexpr reference operator[](size_type index);
    constexpr const_reference operator[](size_type index) const;
    
    constexpr reference At(size_t index);
    constexpr const_reference At(size_t index) const;
    constexpr reference Front();
    constexpr const_reference Front() const;
    constexpr reference Back();
    constexpr const_reference Back() const;
    constexpr void Swap(Array& other) noexcept;
    bool Empty() const noexcept;
    constexpr size_type Size() const noexcept;
    constexpr iterator Data() noexcept;
    constexpr const_iterator Data() const noexcept;
    void Fill(const value_type& value) noexcept;
    
    iterator Begin() noexcept;
    iterator End() noexcept;
    const_iterator Begin() const noexcept;
    const_iterator End() const noexcept;
    const_iterator CBegin() const noexcept;
    const_iterator CEnd() const noexcept;
    
    reverse_iterator RBegin() noexcept;
    reverse_iterator REnd() noexcept;
    const_reverse_iterator CRBegin() const noexcept;
    const_reverse_iterator CREnd() const noexcept;
private:
    value_type _data[N];
};

template <class T, size_t N>
constexpr Array<T, N>::Array(const std::initializer_list<T>& array)
{
    for (size_type i = 0; const auto &elem : array)
    {
        _data[i] = elem;
        ++i;
    }
}

template <class T, size_t N>
constexpr Array<T, N>::Array(const Array& other) noexcept
{
    for (size_type i = 0; i < N; ++i)
        _data[i] = other._data[i];
}

template <class T, size_t N>
constexpr Array<T, N>::Array(Array&& other) noexcept
{
    for (size_type i = 0; i < N; ++i)
        _data[i] = std::move(other._data[i]);
}

template <class T, size_t N>
constexpr Array<T, N>& Array<T, N>::operator=(const Array& other) noexcept
{
    if (this == &other) // object = object
        return *this;

    for (size_type i = 0; i < N; ++i)
        _data[i] = other._data[i];
    
    return *this;
}

template <class T, size_t N>
constexpr Array<T, N>& Array<T, N>::operator=(Array&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    for (size_type i = 0; i < N; ++i)
        _data[i] = other._data[i];
    
    return *this;
}

template <class T, size_t N>
constexpr bool Array<T, N>::operator==(const Array& other) const
{
    if (Size() != other.Size())
        return false;
    
    for (auto it = CBegin(), it_other = other.Begin(); it != End(); ++it, ++it_other)
    {
        if (*it != *it_other)
            return false;
    }
    
    return true;
}

template <class T, size_t N>
constexpr bool Array<T, N>::operator!=(const Array& other) const
{
    return !(*this == other);
}

template <class T, size_t N>
constexpr Array<T, N>::reference Array<T, N>::operator[](size_type index)
{
    return _data[index];
}

template <class T, size_t N>
constexpr Array<T, N>::const_reference Array<T, N>::operator[](size_type index) const
{
    return _data[index];
}

template <class T, size_t N>
constexpr Array<T, N>::reference Array<T, N>::At(size_t index)
{
    if (index >= N)
        throw std::out_of_range("Index is out of range!");
    
    return _data[index];
}

template <class T, size_t N>
constexpr Array<T, N>::const_reference Array<T, N>::At(size_t index) const
{
    if (index >= N)
        throw std::out_of_range("Index is out of range!");
    
    return _data[index];
}

template <class T, size_t N>
constexpr Array<T, N>::reference Array<T, N>::Front()
{
    return _data[0];
}

template <class T, size_t N>
constexpr Array<T, N>::const_reference Array<T, N>::Front() const
{
    return _data[0];
}

template <class T, size_t N>
constexpr Array<T, N>::reference Array<T, N>::Back()
{
    return _data[N - 1];
}

template <class T, size_t N>
constexpr Array<T, N>::const_reference Array<T, N>::Back() const
{
    return _data[N - 1];
}

template <class T, size_t N>
constexpr void Array<T, N>::Swap(Array& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    for (size_type i = 0; i < N; ++i)
        std::swap(_data[i], other._data[i]);
}

template <class T, size_t N>
bool Array<T, N>::Empty() const noexcept
{
    return Size() == 0;
}

template <class T, size_t N>
constexpr size_t Array<T, N>::Size() const noexcept
{
    return N;
}

template <class T, size_t N>
constexpr Array<T, N>::iterator Array<T, N>::Data() noexcept
{
    return _data;
}

template <class T, size_t N>
constexpr Array<T, N>::const_iterator Array<T, N>::Data() const noexcept
{
    return _data;
}

template <class T, size_t N>
void Array<T, N>::Fill(const value_type& value) noexcept
{
    for (size_type i = 0; i < N; ++i)
        _data[i] = value;
}

template <class T, size_t N>
Array<T, N>::iterator Array<T, N>::Begin() noexcept
{
    return iterator(Data());
};

template <class T, size_t N>
Array<T, N>::iterator Array<T, N>::End() noexcept
{
    return iterator(Data() + N);
};

template <class T, size_t N>
Array<T, N>::const_iterator Array<T, N>::Begin() const noexcept
{
    return const_iterator(Data());
};

template <class T, size_t N>
Array<T, N>::const_iterator Array<T, N>::End() const noexcept
{
    return const_iterator(Data() + N);
};

template <class T, size_t N>
Array<T, N>::const_iterator Array<T, N>::CBegin() const noexcept
{
    return Begin();
};

template <class T, size_t N>
Array<T, N>::const_iterator Array<T, N>::CEnd() const noexcept
{
    return End();
};

template <class T, size_t N>
Array<T, N>::reverse_iterator Array<T, N>::RBegin() noexcept
{
    return reverse_iterator(&_data[0] + N - 1);
}

template <class T, size_t N>
Array<T, N>::reverse_iterator Array<T, N>::REnd() noexcept
{
    return reverse_iterator(&_data[0] - 1);
}

template <class T, size_t N>
Array<T, N>::const_reverse_iterator Array<T, N>::CRBegin() const noexcept
{
    return const_reverse_iterator(&_data[0] + N - 1);
}

template <class T, size_t N>
Array<T, N>::const_reverse_iterator Array<T, N>::CREnd() const noexcept
{
    return const_reverse_iterator(&_data[0] - 1);
}

namespace massive
{
    template <class T, size_t N>
    T* Begin(T (&array)[N])
    {
        return array;
    }

    template <class T, size_t N>
    T* End(T (&array)[N])
    {
        return array + N;
    }

    template<class T, size_t N>
    ReverseIterator<T> RBegin(T (&array)[N])
    {
        return ReverseIterator<T>(&array[0] + N - 1);
    }

    template<class T, size_t N>
    ReverseIterator<T> REnd(T (&array)[N])
    {
        return ReverseIterator<T>(&array[0] - 1);
    }
}

#endif /* Header_h */
