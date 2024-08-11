#ifndef Custom_String_h
#define Custom_String_h

#include <array>
#include <iostream>
#include <variant>


class Custom_String
{
    struct big_string
    {
        std::unique_ptr<char[]> data; // 8 байт
        size_t capacity; // 8 байт
    };
    
    struct small_string
    {
        std::array<char, sizeof(big_string)> data; // 16 байт
    };
    
    static constexpr std::size_t sso = sizeof(big_string);
    
public:
    explicit Custom_String(const char* data, std::size_t size):
    _size(size)
    {
        if (Is_SSO()) // 15, т.к. на конце должен (\0)
        {
            small_string small;
            for (size_t i = 0; i < _size; ++i)
            {
                small.data[i] = data[i];
            }
            impl.emplace<small_string>(std::move(small));
        }
        else
        {
            big_string big;
            big.capacity = _size * 2;
            big.data = std::make_unique<char[]>(big.capacity);
            for (size_t i = 0; i < _size; ++i)
            {
                big.data[i] = data[i];
            }
            impl.emplace<big_string>(std::move(big));
        }
    }
    
    explicit Custom_String(const char* data) : Custom_String(data, strlen(data)) {}
    
    ~Custom_String()
    {
        
    }
    
    char* Data() noexcept
    {
        return Is_SSO() ? std::get<small_string>(impl).data.data() : std::get<big_string>(impl).data.get();
    }
    
private:
    bool Is_SSO() const noexcept
    {
        return _size < sso;
    }
private:
    std::size_t _size = 0;
    std::variant<big_string, small_string> impl;
};

#endif /* Custom_String_h */
