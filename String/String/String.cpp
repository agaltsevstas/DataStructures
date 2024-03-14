#include <iostream>
#include <array>

/*
 Сайты:
        https://habr.com/ru/companies/oleg-bunin/articles/352280/
        https://rrmprogramming.com/article/small-string-optimization-sso-in-c/#:~:text=Small%20string%20optimization%20(SSO)%20is,memory%20consumption%20and%20code%20performance
*/

class String
{
private:
    std::unique_ptr<char[]> _data;
    std::size_t _size;
    std::size_t _capacity;
};

class String_SSO
{
private:
    std::unique_ptr<char[]> _data;
    std::size_t _size;
    std::size_t _capacity;
    std::array<char, 16> _sso_buffer;
};

struct string_sso
{
    std::size_t capacity;
    union
    {
        struct
        {
            std::unique_ptr<char[]> data;
            std::size_t size;
        } heap_buffer;
        std::array<char, sizeof(heap_buffer)> sso_buffer;
    };
};


int main()
{
    return 0;
}
