#include "function.h"

#include <iostream>

/*
 Сайты: https://gist.github.com/Junch/3ac1f1d99c8f88f7d2333062d1ebcb2a
        https://github.com/calebh/litefun/blob/master/litefun.cpp
        https://github.com/rigtorp/Function/blob/master/Function.h
        https://github.com/zhihaoy/nontype_functional/blob/main/include/std23/function.h
 */

/*
 std::function - контейнер с фиксированным протитопом, который может содержать объект любого типа и этот объект можно использовать как функцию или как callback. Это единый тип, к которому приводятся все замыкания (closer) с данной сигнатурой. function захватывает функтор/lambda, стирая ее тип: lambda хранится в куче как указатель void* привиденный с помощью reinterpret_cast к типу char[], а сам тип хранится отдельно в шаблонных параметрах.
 ЗАМЫКАНИЕ (closer) (lvalue) - функциональный объект (функтор), создаваемый компилятором при генерации кода. С C++20 можно копировать и создать значение этого типа.
 С помощью связывания (std::bind): можно хранить копии/ссылки/указатели объектов классов, члены классов, аргументы к функциям/методам. С помощью placeholder можно подставлять значения при вызове function.
 
 std::bind - функтор или шаблонная функция, а также инструмент для каррирования и частичного применения функции с несколькими аргументами. bind выделяет память на стеке, а не в куче как function. Функциональный объект (функтор) - класс, у которого переопределен operator(). Объекты этого класса могут вести себя как функция.
 СРАВНЕНИЕ bind и Lambda:
 - bind удобнее использовать, когда много аргументов в фукнции.
 - Lambda удобнее использовать, когда сложное выражение или их несколько.
 
 Каррирование (std::currying) - функция, принимающая только один аргумент, которая продолжает возвращать функции до тех пор, пока не будут отправлены все ее аргументы. В случае, если переданы не все аргументы, функция их запомнит - это возможно с помощью замыкания (closer).
 Частичное применение — это возможность вызывать функции N аргументов передавая в них только часть этих аргументов, результатом такого вызова будет другая функция от оставшихся аргументов.
 Каррирование ≠ частичное применение функции. Отличие: Каррирование принимает по 1 аргументу, частичное применение функции принимает > 1 аргумента.
 
 Плюсы над обычными указателями на функции (function pointer):
 - умеет захватывать переменные контекста [this, &].
 - может работатать как с lambda-функциями, так и с обычными функциями.
 - есть связывание с помощью bind: можно хранить копии/ссылки/указатели объектов классов, члены классов, аргументы к функциям/методам. С помощью placeholder можно подставлять значения при вызове function.
 - может рекурсивно вызывать самого себя.
 
 Минусы над обычными указателями на функции (function pointer):
 - является более затратным по ресурсам, т.к. требуется выделение памяти в куче.
 ОТЛИЧИЕ Lambda от function:
 - имеют разные типы.
 - function захватывает функтор/lambda, стирая ее тип: lambda хранится в куче как указатель void* привиденный с помощью reinterpret_cast к типу char[], а сам тип хранится отдельно в шаблонных параметрах.
 - в function выделение памяти происходит в куче, в lambda - на стеке, поэтому function теряет в производительности.
 - function может вызывать самого себя рекурсивно.
 */

namespace functor
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
};

bool Compare(int lhs, int rhs)
{
    return lhs > rhs;
}


int main()
{
    using namespace functor;
    
    /// Первая реализация - самая правильная
    {
        using namespace first_implementation;
        std::cout << "first implementation" << std::endl;
        
        /// Constructor, Copy constructor, Move constructor, Destructor, Copy operator, Move operator
        {
            function<bool(int, int)> empty;
            
            Greater object;
            function<bool(int, int)> lambda(object);
            lambda = lambda;
            lambda = std::move(lambda);
            auto lambda_copy1(lambda);
            lambda_copy1 = lambda;
            lambda_copy1 = std::move(lambda);
            auto function_copy2(std::move(lambda_copy1));
        }
        /// Рекурсия
        {
            function<int(int)> factorial = [&](int number)
            {
                return (number == 1) ? number : number * factorial(number - 1);
            };
            
            [[maybe_unused]] auto result = factorial(5);
        }
        /// lambda и function имеют разные типы
        {
            [[maybe_unused]] auto closure = [](int lhs, int rhs){ return lhs < rhs;};
            function<bool(int, int)> function = [](int lhs, int rhs){ return lhs < rhs;};
        }
        /// function = функтор
        {
            Greater object;
            function<bool(int, int)> function(object);
            
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
                function<void()> function;
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
                        function<bool()> lambda = std::bind(Compare, 1, 2); // lhs = 1, rhs = 2
                        [[maybe_unused]] auto result = lambda();
                    }
                    /// 2 Способ: bind + placeholder для обоих аргументов
                    {
                        function<bool(int, int)> lambda = std::bind(Compare, std::placeholders::_1, std::placeholders::_2);
                        [[maybe_unused]] auto result = lambda(1, 2); // lhs = 1, rhs = 2
                    }
                    /// 3 Способ: bind + placeholder для левого аргумента
                    {
                        function<bool(int)> lambda = std::bind(Compare, std::placeholders::_1, 2); // rhs = 2
                        [[maybe_unused]] auto result = lambda(1); // lhs = 1
                    }
                    /// 4 Способ: bind + placeholder для правого аргумента
                    {
                        function<bool(int)> lambda = std::bind(Compare, 1, std::placeholders::_1); // lhs = 2
                        [[maybe_unused]] auto result = lambda(1); // rhs = 1
                    }
                    /// 5 Способ: вложенный bind
                    {
                        auto bar = [](int number) noexcept -> int
                        {
                            return number;
                        };
                        
                        function<bool(int, int)> lambda = std::bind(Compare, std::bind(bar, std::placeholders::_1), std::placeholders::_2);
                        
                        lambda(1, 2);
                    }
                }
                /// Связывание метода класса
                {
                    /// 1 Способ: обычный
                    {
                        function<int(const Greater&, int)> lambda;
                        {
                            Greater object;
                            // TODO:
                            // lambda = &Greater::SquareNumber; // 1 Способ
                            lambda = std::bind(&Greater::SquareNumber, std::placeholders::_1, std::placeholders::_2); // 2 Способ
                            std::cout << lambda(object, 1);
                        }
                    }
                    /// 2 Способ: связывание аргумента функции
                    {
                        function<int(const Greater&)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, std::placeholders::_1, 2); // 2 Способ
                            std::cout << lambda(object);
                        }
                    }
                    /// 3 Способ:  связывание копии объекта класса
                    {
                        function<int(int)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, object, std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 4 Способ: связывание указателя на объект класса, но объект может разрушиться раньше времени
                    {
                        function<int(int)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, &object, std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 5 Способ: связывание умного указателя на объект класса, объект НЕ разрушиться раньше времени
                    {
                        function<int(int)> lambda;
                        {
                            auto object = std::make_shared<Greater>();
                            lambda = std::bind(&Greater::SquareNumber, object, std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 6 Способ: связывание ссылку на объект класса, но объект может разрушиться раньше времени
                    {
                        function<int(int)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, std::ref(object), std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 7 Способ: связывание копии объекта класса + аргумента функции
                    {
                        function<int()> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, object, 2);
                        }
                        
                        lambda();
                    }
                }
                /// Связывание члена класса
                {
                    function<int(const Greater&)> lambda;
                    {
                        Greater object;
                        // TODO:
                        // lambda = &Greater::number; // 1 Способ
                        auto lambda = std::bind(&Greater::number, std::placeholders::_1); // 2 Способ
                        [[maybe_unused]] auto number = lambda(object);
                    }
                }
            }
        }
    }
    /// Вторая реалализация
    {
        using namespace second_implementation;
        std::cout << "second implementation" << std::endl;
        
        /// Constructor, Copy constructor, Move constructor, Destructor, Copy operator, Move operator
        {
            // function<bool(int, int)> empty;
            
            Greater object;
            function<bool(int, int)> function(object);
            function = function;
            function = std::move(function);
            auto function_copy1(function);
            function_copy1 = function;
            function_copy1 = std::move(function);
            auto function_copy2(std::move(function_copy1));
        }
        /// Рекурсия
        {
            function<int(int)> factorial = [&](int number)
            {
                return (number == 1) ? number : number * factorial(number - 1);
            };
            
            [[maybe_unused]] auto result = factorial(5);
        }
        /// lambda и function имеют разные типы
        {
            [[maybe_unused]] auto closure = [](int lhs, int rhs){ return lhs < rhs;};
            function<bool(int, int)> function = [](int lhs, int rhs){ return lhs < rhs;};
        }
        /// function = функтор
        {
            Greater object;
            function<bool(int, int)> function(object);
            
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
                function<void()> function;
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
                        function<bool()> lambda = std::bind(Compare, 1, 2); // lhs = 1, rhs = 2
                        [[maybe_unused]] auto result = lambda();
                    }
                    /// 2 Способ: bind + placeholder для обоих аргументов
                    {
                        function<bool(int, int)> lambda = std::bind(Compare, std::placeholders::_1, std::placeholders::_2);
                        [[maybe_unused]] auto result = lambda(1, 2); // lhs = 1, rhs = 2
                    }
                    /// 3 Способ: bind + placeholder для левого аргумента
                    {
                        function<bool(int)> lambda = std::bind(Compare, std::placeholders::_1, 2); // rhs = 2
                        [[maybe_unused]] auto result = lambda(1); // lhs = 1
                    }
                    /// 4 Способ: bind + placeholder для правого аргумента
                    {
                        function<bool(int)> lambda = std::bind(Compare, 1, std::placeholders::_1); // lhs = 2
                        [[maybe_unused]] auto result = lambda(1); // rhs = 1
                    }
                    /// 5 Способ: вложенный bind
                    {
                        auto bar = [](int number) noexcept -> int
                        {
                            return number;
                        };
                        
                        function<bool(int, int)> lambda = std::bind(Compare, std::bind(bar, std::placeholders::_1), std::placeholders::_2);
                        
                        lambda(1, 2);
                    }
                }
                /// Связывание метода класса
                {
                    /// 1 Способ: обычный
                    {
                        function<int(const Greater&, int)> lambda;
                        {
                            Greater object;
                            // TODO:
                            // lambda = &Greater::SquareNumber; // 1 Способ
                            lambda = std::bind(&Greater::SquareNumber, std::placeholders::_1, std::placeholders::_2); // 2 Способ
                            std::cout << lambda(object, 1);
                        }
                    }
                    /// 2 Способ: связывание аргумента функции
                    {
                        function<int(const Greater&)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, std::placeholders::_1, 2); // 2 Способ
                            std::cout << lambda(object);
                        }
                    }
                    /// 3 Способ:  связывание копии объекта класса
                    {
                        function<int(int)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, object, std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 4 Способ: связывание указателя на объект класса, но объект может разрушиться раньше времени
                    {
                        function<int(int)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, &object, std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 5 Способ: связывание умного указателя на объект класса, объект НЕ разрушиться раньше времени
                    {
                        function<int(int)> lambda;
                        {
                            auto object = std::make_shared<Greater>();
                            lambda = std::bind(&Greater::SquareNumber, object, std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 6 Способ: связывание ссылку на объект класса, но объект может разрушиться раньше времени
                    {
                        function<int(int)> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, std::ref(object), std::placeholders::_1);
                        }
                        
                        lambda(1);
                    }
                    /// 7 Способ: связывание копии объекта класса + аргумента функции
                    {
                        function<int()> lambda;
                        {
                            Greater object;
                            lambda = std::bind(&Greater::SquareNumber, object, 2);
                        }
                        
                        lambda();
                    }
                }
                /// Связывание члена класса
                {
                    function<int(const Greater&)> lambda;
                    {
                        Greater object;
                        // TODO:
                        // lambda = &Greater::number; // 1 Способ
                        auto lambda = std::bind(&Greater::number, std::placeholders::_1); // 2 Способ
                        [[maybe_unused]] auto number = lambda(object);
                    }
                }
            }
        }
    }
    
    return 0;
}
