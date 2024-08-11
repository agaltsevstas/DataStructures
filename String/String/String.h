#ifndef String_h
#define String_h

#include <iostream>

class String
{
    union Impl
    {
        struct big_string
        {
            char* data; // 8 байт
            size_t capacity; // 8 байт
        } big;
        
        struct small_string
        {
            char data[sizeof(big_string)];
        } small;
    } impl;
    
    static constexpr std::size_t sso = sizeof(Impl::big_string);
    
public:
    explicit String(const char* data, std::size_t size):
    _size(size)
    {
        char* buffer = nullptr;
        if (Is_SSO()) // 15, т.к. на конце должен (\0)
        {
            buffer = impl.small.data;
        }
        else
        {
            impl.big.capacity = _size * 2;
            buffer = impl.big.data = new char[impl.big.capacity];
        }
        
        std::copy(data, data + size + 1, buffer); // + 1, т.к. на конце должен (\0)
    }
    
    explicit String(const char* data) : String(data, strlen(data)) {}

    
    ~String()
    {
        if (!Is_SSO())
        {
            delete[] impl.big.data;
        }
    }
    
    char* Data() noexcept
    {
        return Is_SSO() ? impl.small.data : impl.big.data;
    }
    
private:
    bool Is_SSO() const noexcept
    {
        return _size < sso;
    }
    
private:
    std::size_t _size = 0;
};

#endif /* String_h */
