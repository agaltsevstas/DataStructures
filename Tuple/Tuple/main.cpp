#include "Tuple.h"

#include <string>
#include <utility>

/*
 Сайты: https://www.itcodar.com/c-plus-1/template-tuple-calling-a-function-on-each-element.html
        https://infotraining.bitbucket.io/cpp-adv/variadic-templates.html
        https://www.scs.stanford.edu/~dm/blog/param-pack.html
        https://github.com/VladimirBalun/Metaprogramming/blob/master/tuple.hpp
        https://github.com/jfalcou/kumi/blob/main/include/kumi/tuple.hpp
        https://github.com/glebzlat/tuple/blob/main/include/Tuple.hpp
        https://github.com/VladimirBalun/Metaprogramming/blob/master/tuple.hpp
        https://chromium.googlesource.com/external/github.com/domokit/base/+/0032c8e1a72eb85d947d8df8de503caa62b4d0a8/tuple.h
        https://mitchnull.blogspot.com/2012/06/c11-tuple-implementation-details-part-1.html
        https://www.vishalchovatiya.com/variadic-template-cpp-implementing-unsophisticated-tuple/
        https://stackoverflow.com/questions/4041447/how-is-stdtuple-implemented
        https://gist.github.com/seiren-naru-shirayuri/d0493137d688eb91277d00e0ed48b9a2
        https://gist.github.com/ericniebler/fa621a311acd2f0339c57e01824c654c
        https://gist.github.com/IvanVergiliev/9639530
 */


/*
 Tuple (Кортеж) - коллекция элементов с фиксированным размером, содержащая разнородные значения. Для реализации кортежа используется идиома Head/Tail с помощью Variadic Template - шаблон с заранее неизвестным числом аргументов.
 */


int main()
{
    std::cout << "tuple" << std::endl;
    using namespace tuple;
    
    /// first implementation
    {
        using namespace first_implementation;
        std::cout << "first implementation" << std::endl;
        
        static_assert(Tuple<>::value == 0, "must be 0");
        static_assert(Tuple<int, double, std::string&>::value == 3, "must be 3");
        
        Tuple<int, double, char, std::string> tuple(1, 10.0, 'c', "abc");
        Tuple tuple_deduction1{1, 10.0, 'c', "ABC"};
        Tuple tuple_deduction2 = Tuple(1, 10.0, 'c', "ABC");
        
        [[maybe_unused]] auto value_int = Get<0>(tuple);
        [[maybe_unused]] auto value_double = Get<1>(tuple);
        [[maybe_unused]] auto value_char = Get<2>(tuple);
        [[maybe_unused]] auto value_string = Get<3>(tuple);
        [[maybe_unused]] auto make_tuple = Make_Tuple(1, 10.0, 'c');
        [[maybe_unused]] auto size = tuple.Size();
        [[maybe_unused]] auto ebo1 = sizeof(Tuple<dummy<0>, dummy<1>, dummy<2>>); // 1
        [[maybe_unused]] auto ebo2 = sizeof(Tuple<dummy<0>, dummy<0>, dummy<0>>); // 3
    }
    
    /// second implementation
    {
        using namespace second_implementation;
        std::cout << "second implementation" << std::endl;
        
        static_assert(Tuple<>::value == 0, "must be 0");
        static_assert(Tuple<int, double, std::string&>::value == 3, "must be 3");
        
        Tuple<int, double, char, std::string> tuple(1, 10.0, 'c', "abc");
        Tuple tuple_deduction1{1, 10.0, 'c', "ABC"};
        Tuple tuple_deduction2 = Tuple(1, 10.0, 'c', "ABC");
        
        [[maybe_unused]] auto value_int = Get<0>(tuple);
        [[maybe_unused]] auto value_double = Get<1>(tuple);
        [[maybe_unused]] auto value_char = Get<2>(tuple);
        [[maybe_unused]] auto value_string = Get<3>(tuple);
        [[maybe_unused]] auto make_tuple = Make_Tuple(1, 10.0, 'c');
        [[maybe_unused]] auto size = tuple.Size();
        [[maybe_unused]] auto ebo1 = sizeof(Tuple<dummy<0>, dummy<1>, dummy<2>>); // 1
        [[maybe_unused]] auto ebo2 = sizeof(Tuple<dummy<0>, dummy<0>, dummy<0>>); // 3
    }
    
    /// third implementation
    {
        using namespace third_implementation;
        std::cout << "third implementation" << std::endl;
        
        static_assert(Tuple<>::value == 0, "must be 0");
        static_assert(Tuple<int, double, std::string&>::value == 3, "must be 3");
        
        Tuple<int, double, char, std::string> tuple(1, 10.0, 'c', "abc");
        Tuple tuple_deduction1{1, 10.0, 'c', "ABC"};
        Tuple tuple_deduction2 = Tuple(1, 10.0, 'c', "ABC");
        
        [[maybe_unused]] auto value_int = Get<0>(tuple);
        [[maybe_unused]] auto value_double = Get<1>(tuple);
        [[maybe_unused]] auto value_char = Get<2>(tuple);
        [[maybe_unused]] auto value_string = Get<3>(tuple);
        [[maybe_unused]] auto make_tuple = Make_Tuple(1, 10.0, 'c');
        [[maybe_unused]] auto size = tuple.Size();
        [[maybe_unused]] auto ebo1 = sizeof(Tuple<dummy<0>, dummy<1>, dummy<2>>); // 1
        [[maybe_unused]] auto ebo2 = sizeof(Tuple<dummy<0>, dummy<0>, dummy<0>>); // 3
        
        [[maybe_unused]] auto make_ref_tuple = Make_Ref_Tuple(value_int, value_double, value_char, value_string);
        Get<0>(make_ref_tuple) = 10;
        Get<1>(make_ref_tuple) = 100.0;
        Get<2>(make_ref_tuple) = 'C';
        Get<3>(make_ref_tuple) = "ABC";
    }
    
    return 0;
}
