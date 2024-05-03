#include "String.h"
#include "Custom_String.h"

#include <string>

/*
 Сайты: https://habr.com/ru/companies/oleg-bunin/articles/352280/
        https://rrmprogramming.com/article/small-string-optimization-sso-in-c/#:~:text=Small%20string%20optimization%20(SSO)%20is,memory%20consumption%20and%20code%20performance
        https://github.com/elliotgoodrich/SSO-23/blob/master/include/string.hpp
        https://github.com/robhz786/api_string/blob/master/include/string.hpp
*/


int main()
{
    std::cout << "std::string: " << sizeof(std::string) << std::endl;
    std::cout << "String: " << sizeof(String) << std::endl;
    std::cout << "Custom_String: " << sizeof(Custom_String) << std::endl;
    std::cout << std::endl;
    
    std::string str1("aaaaaaaaaaaaaaa");
    std::string str2("aaaaaaaaaaaaaaaa");
    String string1("aaaaaaaaaaaaaaa");
    String string2("aaaaaaaaaaaaaaaa");
    Custom_String custom_string1("aaaaaaaaaaaaaaa");
    Custom_String custom_string2("aaaaaaaaaaaaaaaa");
    
    [[maybe_unused]] auto str1_data = str1.data();
    [[maybe_unused]] auto str2_data = str2.data();
    [[maybe_unused]] auto string1_data = string1.Data();
    [[maybe_unused]] auto string2_data = string2.Data();
    [[maybe_unused]] auto custom_string1_data = custom_string1.Data();
    [[maybe_unused]] auto custom_string2_data = custom_string2.Data();
    
    std::cout << "str1: " << sizeof(str1) << std::endl;
    std::cout << "str2: " << sizeof(str2) << std::endl;
    std::cout << "string1: " << sizeof(string1) << std::endl;
    std::cout << "string2: " << sizeof(string2) << std::endl;
    std::cout << "custom_string1: " << sizeof(custom_string1) << std::endl;
    std::cout << "custom_string2: " << sizeof(custom_string2) << std::endl;
    return 0;
}
