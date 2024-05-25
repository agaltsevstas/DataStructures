#include "function.hpp"

#include <functional>
#include <iostream>

namespace FUNCTION
{
    struct Greater
    {
        constexpr bool operator()(int lhs, int rhs) // Может быть возвано во время компиляции
        {
            return lhs > rhs;
        }
        
        int SquareNumber(int number) const { return number * number; }
        
        ~Greater()
        {
            std::cout << "Destructor" << std::endl;
        }
        
        int number = 0;
    };

    bool Compare(int lhs, int rhs)
    {
        return lhs > rhs;
    }

    void Start()
    {
        /// Рекурсия
        {
            std::function<int(int)> factorial = [&](int number)
            {
                return (number == 1) ? number : number * factorial(number - 1);
            };
            
            [[maybe_unused]] auto name_mangling = factorial.target_type().name();
            [[maybe_unused]] auto result = factorial(5);
        }
        /// lambda и function имеют разные типы
        {
            [[maybe_unused]] auto closure = [](int lhs, int rhs){ return lhs < rhs;};
            std::function<bool(int, int)> function = [](int lhs, int rhs){ return lhs < rhs;};
        }
        /// function = функтор
        {
            Greater object;
            std::function<bool(int, int)> function(object);
            
            [[maybe_unused]] auto result1 = function(1, 2);
            [[maybe_unused]] auto result2 = function(2, 1);
        }
        /*
         std::bind - функтор или шаблонная функция, а также инструмент для каррирования и частичного применения функции с несколькими аргументами. bind выделяет память на стеке, а не в куче как function.
         СРАВНЕНИЕ bind и Lambda:
         - bind удобнее использовать, когда много аргументов в фукнции.
         - Lambda удобнее использовать, когда сложное выражение или их несколько.
         */
        {
            /// Неправильное связывание: провисание ссылок, но почему-то все работает!
            {
                std::function<void()> function;
                {
                    Greater object;
                    auto lambda = [&objectRef = object](){ };
                    function = lambda;
                }
                
                function();
            }
            /// Правильное связывание: метод класса
            {
                /// Связывание функции
                {
                    /// 1 Способ: bind
                    {
                        std::function<bool()> lambda = std::bind(Compare, 1, 2); // lhs = 1, rhs = 2
                        [[maybe_unused]] auto result = lambda();
                    }
                    /// 2 Способ: bind + placeholder для обоих аргументов
                    {
                        std::function<bool(int, int)> lambda = std::bind(Compare, std::placeholders::_1, std::placeholders::_2);
                        [[maybe_unused]] auto result = lambda(1, 2); // lhs = 1, rhs = 2
                    }
                    /// 3 Способ: bind + placeholder для левого аргумента
                    {
                        std::function<bool(int)> lambda = std::bind(Compare, std::placeholders::_1, 2); // rhs = 2
                        [[maybe_unused]] auto result = lambda(1); // lhs = 1
                    }
                    /// 4 Способ: bind + placeholder для правого аргумента
                    {
                        std::function<bool(int)> lambda = std::bind(Compare, 1, std::placeholders::_1); // lhs = 2
                        [[maybe_unused]] auto result = lambda(1); // rhs = 1
                    }
                    /// 5 Способ: вложенный bind
                    {
                        auto bar = [](int number) noexcept -> int
                        {
                            return number;
                        };
                        
                        std::function<bool(int, int)> lambda = std::bind(Compare, std::bind(bar, std::placeholders::_1), std::placeholders::_2);
                        
                        lambda(1, 2);
                    }
                }
                /// Связывание метода класса
                {
                    /// 1 Способ: обычный
                    {
                        std::function<int(const Greater&, int)> function;
                        {
                            Greater object;
                            function = &Greater::SquareNumber; // 1 Способ
                            function = std::bind(&Greater::SquareNumber, std::placeholders::_1, std::placeholders::_2); // 2 Способ
                            std::cout << function(object, 1);
                        }
                    }
                    /// 2 Способ: связывание аргумента функции
                    {
                        std::function<int(const Greater&)> function;
                        {
                            Greater object;
                            function = std::bind(&Greater::SquareNumber, std::placeholders::_1, 2); // 2 Способ
                            std::cout << function(object);
                        }
                    }
                    /// 3 Способ:  связывание копии объекта класса
                    {
                        std::function<int(int)> function;
                        {
                            Greater object;
                            function = std::bind(&Greater::SquareNumber, object, std::placeholders::_1);
                        }
                        
                        function(1);
                    }
                    /// 4 Способ: связывание указателя на объект класса, но объект может разрушиться раньше времени
                    {
                        std::function<int(int)> function;
                        {
                            Greater object;
                            function = std::bind(&Greater::SquareNumber, &object, std::placeholders::_1);
                        }
                        
                        function(1);
                    }
                    /// 5 Способ: связывание умного указателя на объект класса, объект НЕ разрушиться раньше времени
                    {
                        std::function<int(int)> function;
                        {
                            auto object = std::make_shared<Greater>();
                            function = std::bind(&Greater::SquareNumber, object, std::placeholders::_1);
                        }
                        
                        function(1);
                    }
                    /// 6 Способ: связывание ссылку на объект класса, но объект может разрушиться раньше времени
                    {
                        std::function<int(int)> function;
                        {
                            Greater object;
                            function = std::bind(&Greater::SquareNumber, std::ref(object), std::placeholders::_1);
                        }
                        
                        function(1);
                    }
                    /// 7 Способ: связывание копии объекта класса + аргумента функции
                    {
                        std::function<int()> function;
                        {
                            Greater object;
                            function = std::bind(&Greater::SquareNumber, object, 2);
                        }
                        
                        function();
                    }
                }
                /// Связывание члена класса
                {
                    std::function<int(const Greater&)> function;
                    {
                        Greater object;
                        function = &Greater::number; // 1 Способ
                        auto function = std::bind(&Greater::number, std::placeholders::_1); // 2 Способ
                        [[maybe_unused]] auto number = function(object);
                    }
                }
            }
        }
    }
}
