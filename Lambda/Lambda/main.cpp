#include "function.hpp"
#include "lambda.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <iterator>
#include <vector>
#include <variant>

/*
 Лекция: https://www.youtube.com/watch?v=GPBY8KNGbhc&ab_channel=selfedu
         https://www.youtube.com/watch?v=CFOIHo5aNTo&ab_channel=selfedu
         https://www.youtube.com/watch?v=AFN0kxXK5qw&t=3506s
         https://www.youtube.com/watch?v=2uIzDBip3dc
         https://www.youtube.com/watch?v=Dojp56LVYKM&t=2664s
         https://www.youtube.com/watch?v=hkDFxqbNlwk&t=3490s&ab_channel=KonstantinVladimirov
         https://www.youtube.com/watch?v=xIMWRUHUznM&ab_channel=KonstantinVladimirov
 */

/*
 Сайты: https://learn.microsoft.com/ru-ru/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170
        https://habr.com/ru/articles/223289/
        https://rigtorp.se/iife/
        https://learn.microsoft.com/ru-ru/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170
        https://stackoverflow.com/questions/77186627/how-to-perform-complex-initialization-of-a-const-data-member-in-the-member-initi
        https://habr.com/ru/articles/340722/
        
 */

/*
 ЛЯМБДА (rvalue) — это просто анонимная функция. Позволяет создавать объект (замыкание) в именную локальную функцию, которую можно создать внутри какой-нибудь функции.
 ЗАМЫКАНИЕ (closer) (lvalue) - функциональный объект (функтор), создаваемый компилятором при генерации кода. С C++20 можно копировать и создать значение этого типа.
 Пример:
 using SQRT = decltype([](int x)
 {
     return std::pow(x, 2);
 });
 SQRT sqrt1; // Создание типа
 auto sqrt2 = sqrt1; // Копирование значение
 1. В списке захвата (capture list) []  при указании переменных - создаются private члены класса с такими же типами и именами. С C++14 можно использовать move семантику.
 2. В списке аргументов (), который является необязательным и можно не писать (), при указании переменных - создается конструктор, куда передаются значения. С C++20 можно передавать шаблоны только в качестве аргументов.
 3. Переопределение operator() const - означает, что в константном методе локальные переменные (члены класса) захваченные по значению менять - нельзя, по ссылке - можно. Обойти это ограничение можно с помощью mutable, которое убирает const у operator() и позволяет изменять копии локальных переменных, но не оригиналы. Для глобальных/статических переменных это правило не распространяется.
 4. Перегружать лямбды - нельзя, но можно можно делать имитацию перегрузки с помощью шаблонных миксин (template mixin).
 ЗАМЕЧАНИЕ: лямбда-функция ≠ замыкание (closer).
 СИНТАКСИС LAMBDA:
 auto lambda - [](){}();
 function - замыкание (closer). Это объект, который имеет тип.
 [] - список захвата (capture list), в котором указываются локальные переменные, которые будут изменяться.
 () - cписок аргументов, если их нет (можно не писать).
 constexpr - может вычисляться на этапе компиляции, но с C++20 идет по-умолчанию (можно не указывать).
 mutable - позволяет изменять копии локальных переменных, но не оригиналы. Для глобальных/статических переменных это правило не распространяется (mutable можно не указывать).
 noexcept - спецификатор, указывающий что лямбда-выражение не создает никаких исключений (можно не указывать).
 -> - выводимый тип (можно не указывать).
 {} - тело функции.
 Immediate invocation: вызов безымянной функции:
 [](){}();
 auto lambda2 = lambda; // lambda и lambda2 - два разных объекта
 */

struct Myclass
{
public:
    int Lambda() // Захват членов класса
    {
        // return [this]() { _number = 11; return _number; }(); // Может быть Undefined behavior, потому что объект может разрушен раньше времени
        
        // return [=]() { _number = 11; return _number; }(); // Захват по значению (по умолчанию), включая this: может быть Undefined behavior, потому что объект может разрушен раньше времени
        
        // return [&]() { _number = 11; return _number; }(); // Захват по ссылке (по умолчанию): может быть Undefined behavior, потому что объект может разрушен раньше времени
        
        return [*this]() mutable { _number = 11; return _number; }(); // C++17: Копирование всех членов класса, чтобы не было Undefined behavior при разрушении объекта раньше времени
        
        return [number = _number]() mutable { number = 11; return number; }(); // Копирование члена класса в локальную переменную, чтобы не было Undefined behavior при разрушении объекта раньше времени
    }
    
private:
    int _number = 10;
};

template <typename String, typename ...Strings>
requires std::is_same_v<String, std::vector<std::string>> && ((std::is_same_v<Strings, std::string>) && ...)
void FindSubString(const String& vec, Strings&& ...subs)
{
    auto contains_substring = [&subs...](const std::string& str)
    {
        return ((str.find(subs) != std::string::npos) && ...);
    };
    
    auto iter = std::find_if(vec.begin(), vec.end(), contains_substring);
    if (iter != vec.end())
        std::cout << "Найдена подстрока: " << *iter << std::endl;
}

namespace MIXINS
{
    template <typename... Mixins>
    struct Mixin : Mixins...
    {
        using Mixins::operator()...;
    };

    /// Deduction hints: нет лишнего создания объектов и лишней передачив конструктор, создается только 1 объект и вызывается 1 конструктор
    template <typename ...Mixins> Mixin(Mixins...) -> Mixin<Mixins...>;
}

int Function(int a, int b, const std::function<int(int, int)>& lambda)
{
    return lambda(a, b);
}

constexpr int number1 = 10;
int number2 = 10;

int main()
{
    /// Lambda под капотом
    {
        /*
         Компилятор при генерации кода создает функциональный объект (функтор), в списке захвата [] (capture list)  при указании переменных - создаются private члены класса с такими же типами и именами. Переопределение operator() const - означает, что в константном методе переменные захваченные по значению менять - нельзя, по ссылке - можно. Обойти это ограничение можно с помощью mutable, которое убирает const у operator(). Перегружать лямбды - нельзя.
         */
        const int number1 = 1;
        const int number2 = 2;
        int number3 = 3;
        int number4 = 4;
        
        auto function = [number1, &number2, number3, &number4](auto lhs, auto rhs)
        {
            // number1 = 1; // Ошибка: Cannot assign to non-static data member 'number1' with const-qualified type 'const int'
            // number2 = 2; // Ошибка: Cannot assign to non-static data member 'number2' with const-qualified type 'const int &'
            // number3 = 3; // Ошибка: Cannot assign to non-static data member within const member function 'operator()'
            number4 = 4;
            return lhs < rhs;
        };
        
        [[maybe_unused]] auto result1 = function(1, 2);
        // То же самое
        [[maybe_unused]] auto result2 = LAMBDA::Lambda(number1, number2, number3, number4)(1, 2);
    }
    /// Immediate invocation: вызов безымянной функции
    {
        [](){}(); // Со списком аргументов
        
        []{}(); // Без списка аргументов
        
        []() // Не вызовется, т.к. не добавили вызов после определения
        {
            std::cout << "Безымянная функция" << std::endl;
        };
        
        []() // Вызовется, т.к. добавили вызов после определения
        {
            std::cout << "Безымянная функция" << std::endl;
        }();
        
        [](int number) // Вызовется с аргументом, т.к. добавили вызов после определения
        {
            std::cout << "Безымянная функция" << std::endl;
        }(10);
    }
    /// C++11
    {
        static int number3 = 10;
        int a = 1;
        int b = 2;
        int c = 3;
        int* ptr = &c;
        std::string str = "str";
        
        auto lambda1 = []() // Список захвата (capture list) пустой: изменять локальные переменные - нельзя, глобальные/статические - можно:
        {
            // a; // Локальная переменная недоступна
            // b; // Локальная переменная недоступна
            // ptr; // Локальный указатель недоступен
            number1; // Глобальная переменная доступна
            number2 = 11; // Глобальная константная переменная доступна
            number3 = 11; // Статическая переменная доступна
        };
        
        auto lambda2 = [&] // НЕ РЕКОМЕНДУЕТСЯ: Захват всех локальных переменных по ссылке, изменять - можно. Компилятор может оптимизировать неиспользуемые переменные и тогда они в lambda не попадут:
        {
            a = 1; // Передача локальной переменной по ссылке, изменять можно
            b = 2; // Передача локальной переменной по ссылке, изменять можно
            ptr = &c; // Передача локального указателя, изменять можно адрес
            ++(*ptr); // Передача локального указателя, изменять можно значение
            number2 = 12; // Передача глобальной переменной по ссылке, изменять можно
            number3 = 12; // Передача статической переменной по ссылке, изменять можно
            return (a + b) * c;
        };
        
        auto lambda3 = [=] // НЕ РЕКОМЕНДУЕТСЯ: Захват всех локальных переменных по значению (копирование), изменять - нельзя, глобальные/статические изменять - можно. Компилятор может оптимизировать неиспользуемые переменные и тогда они в lambda не попадут:
        {
            // a = 1;  // Передача локальной переменной по значению (копирование), изменять нельзя
            // b = 2; // Передача локальной переменной по значению (копирование), изменять нельзя
            // ptr = nullptr; // Передача локального указателя, изменять нельзя адрес
            ++(*ptr); // Передача локального указателя, изменять можно
            number2 = 13; // Передача глобальной переменной по ссылке, изменять можно
            number3 = 13; // Передача статической переменной по ссылке, изменять можно
        };
        
        auto lambda4 = [=, &a] // Смешенный захват: захват всех локальных переменных по значению кроме a - по ссылке; а изменять - можно, b изменять - нельзя, глобальные/статические изменять - можно:
        {
            a = 1;  // Передача локальной переменной по ссылке, изменять можно
            // b = 2; // Передача локальной переменной по значению (копирование), изменять нельзя
            // ptr = nullptr; // Передача локального указателя, изменять нельзя адрес
            ++(*ptr); // Передача локального указателя, изменять можно
            number2 = 14; // Передача глобальной переменной по ссылке, изменять можно
            number3 = 14; // Передача статической переменной по ссылке, изменять можно
        };
        
        auto lambda5 = [&, b, ptr] // Смешанный захват: захват всех локальных переменных по ссылке кроме b - по значению (копирование), указатель ptr по значению (копирование); а изменять можно, b изменять нельзя:
        {
            a = 1;  // Передача локальной переменной по ссылке, изменять можно
            // b = 2; // Передача локальной переменной по значению (копирование), изменять нельзя
            // ptr = nullptr; // Передача локального указателя, изменять нельзя адрес
            ++(*ptr); // Передача локального указателя, изменять можно
            number2 = 14; // Передача глобальной переменной по ссылке, изменять можно
            number3 = 14; // Передача статической переменной по ссылке, изменять можно
        };
        
        auto lambda6 = [=]() mutable // Захват всех локальных переменных по значению (копирование), изменения для локальных переменных при выходе из функции - не сохраняться, для глобальных/статических - сохраняется:
        {
            a = 2;  // Передача локальной переменной по значению (копирование), изменять можно, изменения не сохранятся
            b = 3; // Передача локальной переменной по значению (копирование), изменять можно, изменения не сохранятся
            ptr = &c; // Передача локального указателя, изменять можно адрес, изменения сохранятся
            ++(*ptr); // Передача локального указателя, изменять можно, изменения сохранятся
            number2 = 15; // Передача глобальной переменной по ссылке, изменять можно, изменения сохранятся
            number3 = 15; // Передача статической переменной по ссылке, изменять можно, изменения сохранятся
        };
        
        auto lambda7 = [i = 0]() mutable // Инициализация объекта в списке захвата (campture list)
        {
            std::cout << ++i << std::endl;
        };
        
        auto lambda8 = [&str_copy = str]() // РЕКОМЕНДУЕТСЯ: Инициализация объекта по ссылке в списке захвата
        {
            std::cout << str_copy << std::endl;
        };
        
        auto lambda9 = [&str = std::as_const(str)]() // РЕКОМЕНДУЕТСЯ: Инициализация объекта по константной ссылке в списке захвата
        {
            std::cout << str << std::endl;
        };
        
        auto lambda10 = [str = std::cref(str)]() // РЕКОМЕНДУЕТСЯ: Инициализация объекта по константной ссылке в списке захвата
        {
            std::cout << str.get() << std::endl;
        };
        
        auto lambda11 = [&a]()->int // Захват переменной int а по ссылке, а изменять можно, возврат в int
        {
            a = 1.1;  // Передача локальной переменной по ссылке, изменять можно во float, но это не изменит тип переменной
            return a;
        };
        
        std::function<int(int, int)> lambda12;
        lambda12 = [&c](int a, int b) // // Захват переменной c по ссылке
        {
            std::cout << "a: " << a << " b: " << b << ", Result: ";
            return (a + b) * c;
        };
        
        lambda1();
        lambda2();
        lambda3();
        lambda4();
        lambda5();
        lambda6();
        lambda7();
        lambda8();
        lambda9();
        lambda10();
        lambda11();
        Myclass myclass;
        std::cout << "Lambda функция объекта класса Myclass: " << myclass.Lambda() << std::endl;
        std::cout << "Lambda функция объекта класса Myclass: " << Myclass{}.Lambda() << std::endl; // Может быть Undefined behavior
        std::cout << "f1(Sum): " << Function(a, b, lambda12) << std::endl;
        std::cout << "----------" << std::endl;
        std::cout << "f1(Sum & Multi): " << Function(a, b, [&c](int a, int b)
                                                     {
            std::cout << "a: " << a << " b: " << b << ", Result: ";
            return (a + b) * c;
        }) << std::endl;
        std::cout << "----------" << std::endl;
    }
    /// C++14
    {
        /// auto в аргументах
        {
            auto lambda = [](auto lhs, auto rhs) -> auto
            {
                return lhs + rhs;
            };
            
            [[maybe_unused]] auto result1 = lambda(1, 2);
            [[maybe_unused]] auto result2 = lambda(1.0, 2.0);
            [[maybe_unused]] auto result3 = lambda(std::string("1"), std::string("2"));
        }
        /// move в списке захвата (capture list)
        {
            auto str = std::make_unique<std::string>("str");
            auto lambda = [str = std::move(str)]
            {
                
            };
            
            lambda();
        }
    }
    /// C++17: Lambda может быть constexpr, но с C++20 идет по-умолчанию, так что писать необязательно
    {
        constexpr int y = 32;
        /// 1 Способ
        {
            constexpr auto lambda = [y]()
            {
                constexpr int x = 10;
                return x + y;
            };
            
            [[maybe_unused]] auto result = lambda();
        }
        /// 2 Способ
        {
            auto lambda = [y]()
            {
                constexpr int x = 10;
                return x + y;
            };
            
            [[maybe_unused]] auto result = lambda();
        }
    }
    /// C++20
    {
        /// Lambda можно передавать шаблоны только в качестве аргументов
        {
            /// 1 Способ: обычный
            {
                auto lambda = []<typename T>(const T& number)
                {
                    return number;
                };
                
                [[maybe_unused]] auto result = lambda(10);
            }
            /// 2 Способ: сокращенный шаблон (auto)
            {
                auto lambda = [&](auto&& number)
                {
                    return number;
                };
                
                [[maybe_unused]] auto result = lambda(10);
            }
        }
        /// Lambda можно копировать и создать значение этого типа
        {
            using SQRT = decltype([](int x)
                                  {
                return std::pow(x, 2);
            });
            
            SQRT sqrt1; // Создание типа
            auto sqrt2 = sqrt1; // Копирование значение
            
            [[maybe_unused]] auto result1 = sqrt1(1);
            [[maybe_unused]] auto result2 = sqrt2(10);
        }
    }
    /*
     Каррирование (std::currying) - функция, принимающая только один аргумент, которая продолжает возвращать функции до тех пор, пока не будут отправлены все ее аргументы. В случае, если переданы не все аргументы, функция их запомнит - это возможно с помощью замыкания (closer). std::bind - инструмент для каррирования и частичного применения функции.
     Плюсы:
     - код более лаконичный и понятный
     - уменьшение количество повторяющихся аргументов в функции
     - можно запоминать аргументы
     ЗАМЕЧАНИЕ: каррирование ≠ частичное применение функции. Отличие: Каррирование принимает по 1 аргументу, частичное применение функции принимает > 1 аргумента.
     */
    {
        /// 1 Способ: обычный
        {
            auto sum = [](int lhs, int rhs)
            {
                return lhs + rhs;
            };
            
            auto curry = [&sum](int lhs)
            {
                return [=](int rhs)
                {
                    return sum(lhs, rhs);
                };
            };
            
            [[maybe_unused]] auto curry1 = curry(1); // запоминаем 1 аргумент
            [[maybe_unused]] auto curry2 = curry(1)(2); // // вызов с 2 аргументов, 1 аргумент запоминили
            // [[maybe_unused]] auto curry2 = curry(1)(2)(3); // Ошибка: вызов с 3 аргументами
            
            [[maybe_unused]] auto result1 = sum(1, 2);
            [[maybe_unused]] auto result2 = curry1(2); // вызов с 2 аргументов, 1 аргумент запоминили
        }
        /// 2 Способ: Variadic Template - шаблон с заранее неизвестным числом аргументов
        {
            auto curry = []<typename Function, typename... Args>(const Function& function, Args&& ...args)
            {
                return [=](auto... rest) // Захватили по значению функцию и все аргументы и из всех оставшихся выведены rest
                {
                    return function(args..., std::forward<decltype(rest)>(rest)...);
                };
            };
            
            auto function = [](auto x, auto y, auto z) { return x * (y + z); };
            auto curry1 = curry(function, 1); // 1 * (y + z)
            auto curry2 = curry(function, 1, 2); // 1 * (2 + z)
            auto curry3 = curry(function, 1, 2, 3); // 1 * (2 + 3)
            
            [[maybe_unused]] auto result1 = curry1(2, 3);
            [[maybe_unused]] auto result2 = curry2(3);
            [[maybe_unused]] auto result3 = curry3();
        }
    }
    /*
     Частичное применение функции - функция с меньшим количеством аргументов, чем она ожидает, и возвращает функцию, беря остальные аргументы.
     Плюсы:
     - код более лаконичный и понятный
     - уменьшение количество повторяющихся аргументов в функции
     - можно запоминать аргументы
     Каррирование ≠ частичное применение функции. Отличие: Каррирование принимает по 1 аргументу, частичное применение функции принимает > 1 аргумента.
     */
    {
        auto sum = [](int one, int two, int three)
        {
            return one + two + three;
        };
        
        auto curry = [&sum](int one, int two)
        {
            return [=](int three)
            {
                return sum(one, two, three);
            };
        };
        
        auto curry1 = curry(1, 2); // запоминаем 2 аргумента
        
        [[maybe_unused]] auto result1 = curry(1, 2)(3); // вызов с 3 аргументами
        [[maybe_unused]] auto result2 = curry1(3); // вызов с 1 аргументом, 2 аргумента запоминили
    }
    /*
     Идиома IILE (immediately invoked lambda expression) - для инициализации константного объекта создается безымянная lambda-функция, и на месте ее создания - она вызывается. Идиома удобна для замера конструирования объекта или для инициализации константной переменной в конструкторе с использованием сложной логики.
     */
    {
        struct A
        {
            int number = 0;
        };
        
        /// Без IILE
        {
            std::cout << "Без IILE" << std::endl;
            
            class B
            {
            public:
                B(A* a) : _isNull(a && a->number == 0)
                {
                    std::cout << "isNull: " << _isNull << std::endl;
                }
                
            private:
                const bool _isNull;
            };
            
            A a1;
            A a2{10};
            
            B b1(nullptr);
            B b2(&a1);
            B b3(&a2);
            
            std::cout << std::endl;
        }
        /// С IILE
        {
            std::cout << "С IILE" << std::endl;
            /// Инициализация константной переменной в конструкторе с использованием сложной логики
            {
                std::cout << "Инициализация константной переменной в конструкторе с использованием сложной логики" << std::endl;
                
                class B
                {
                public:
                    B(A* a) : _isNull([a] {
                        return a && a->number == 0;
                    }())
                    {
                        std::cout << "isNull: " << _isNull << std::endl;
                    }
                    
                private:
                    const bool _isNull;
                };
                
                A a1;
                A a2{10};
                
                B b1(nullptr);
                B b2(&a1);
                B b3(&a2);
            }
            /// Инициализация константной переменной
            {
                std::cout << "Инициализация константной переменной" << std::endl;
                
                const std::vector<int> numbers = []()
                {
                    std::vector<int> numbers{2,5,1,4,3};
                    std::cout << "vector before sort" << std::endl;
                    for (const auto& n : numbers)
                    {
                        std::cout << n;
                        if (n != numbers.back())
                            std::cout << ", ";
                    }
                    std::cout << std::endl;
                    std::sort(numbers.begin(), numbers.end());
                    return numbers;
                }();
                
                std::cout << "vector after sort" << std::endl;
                for (const auto& n : numbers)
                {
                    std::cout << n;
                    if (n != numbers.back())
                        std::cout << ", ";
                }
                
                std::cout << std::endl;
            }
        }
    }
    /// lambda в качестве компаратора
    {
        std::vector numbers = {1, 4, 3, 2, 5};
        std::sort(numbers.begin(), numbers.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs < rhs;
        });
    }
    /// Инкрементирование с помощью lambda: инициализация объекта в списке захвата (campture list)
    {
        std::vector<size_t> sizeNumbers;
        sizeNumbers.resize(10);
        std::generate(sizeNumbers.begin(), sizeNumbers.end(), [i = 0]() mutable { return ++i; });
    }
    /// lambda в качестве сравнения для сортировки по длине слова для std::set
    {
        std::cout << "Сортировка по длине слова" << std::endl;
        std::vector<std::string> words = { "Please", "replace", "the", "vowels", "in", "this", "sentence", "by", "asterisks" };
        /// 1 Способ
        {
            std::cout << "1 Способ: ";
            auto compare = [](const std::string& lhs, const std::string& rhs)
            {
                return lhs.size() < rhs.size();
            };
            
            std::set<std::string, decltype(compare)> sortedWords;
            std::copy(words.begin(), words.end(), std::inserter(sortedWords, sortedWords.begin()));
            std::copy(sortedWords.begin(), sortedWords.end(), std::ostream_iterator<std::string>(std::cout, " "));
            std::cout << std::endl;
        }
        /// 2 Способ
        {
            std::cout << "2 Способ: ";
            std::set<std::string, decltype([](const std::string& lhs, const std::string& rhs) {return lhs.size() < rhs.size();})> sortedWords;
            std::copy(words.begin(), words.end(), std::inserter(sortedWords, sortedWords.begin()));
            std::copy(sortedWords.begin(), sortedWords.end(), std::ostream_iterator<std::string>(std::cout, " "));
            std::cout << std::endl;
        }
    }
    /// Имитация перегрузки
    {
        using namespace MIXINS;
        
        /// 1 Способ: обычный
        {
            Mixin{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                  [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                  [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                  [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                  [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
            }(1, 1, 1);
        }
        
        /// 2 Способ: std::variant + std::visit
        {
            std::vector<std::variant<int, double, std::string>> vec = {10, 10.0, "str"};
            for (const auto& v : vec)
            {
                std::visit(Mixin{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                                 [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                                 [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                                 [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                                 [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                }, v);
            }
        }
        /// 3 Способ: несколько std::variant + std::visit
        {
            std::variant<int, double, std::string> var1, var2, var3;
            var1 = 10;
            var2 = 10.0;
            var3 = "str";
            std::visit(Mixin{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                             [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                             [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                             [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                             [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
            }, var1, var2, var3);
        }
    }
    /// Lambda +  concept
    {
        std::vector<std::string> vec = {"str1", "str2", "str3"};
        FindSubString(vec, std::string("str"), std::string("str1"));
    }
    /*
     std::function - контейнер с фиксированным протитопом, который может содержать объект любого типа и этот объект можно использовать как функцию или как callback. Это единый тип, к которому приводятся все замыкания (closer) с данной сигнатурой. function захватывает функтор/lambda, стирая ее тип: lambda хранится в куче как указатель void* привиденный с помощью reinterpret_cast к типу char[], а сам тип хранится отдельно в шаблонных параметрах.
     ЗАМЫКАНИЕ (closer) (lvalue) - функциональный объект (функтор), создаваемый компилятором при генерации кода. С C++20 можно копировать и создать значение этого типа.
     С помощью связывания (std::bind): можно хранить копии/ссылки/указатели объектов классов, члены классов, аргументы к функциям/методам. С помощью placeholder можно подставлять значения при вызове function.
     
     std::bind - функтор или шаблонная функция, а также инструмент для каррирования и частичного применения функции с несколькими аргументами. bind выделяет память на стеке, а не в куче как function. Функциональный объект (функтор) - класс, у которого переопределен оператор (). Объекты этого класса могут вести себя как функция.
     СРАВНЕНИЕ bind и Lambda:
     - bind удобнее использовать, когда много аргументов в фукнции.
     - Lambda удобнее использовать, когда сложное выражение или их несколько.
     
     Каррирование (std::currying) - функция, принимающая только один аргумент, которая продолжает возвращать функции до тех пор, пока не будут отправлены все ее аргументы. В случае, если переданы не все аргументы, функция их запомнит - это возможно с помощью замыкания (closer).
     Частичное применение — это возможность вызывать функции N аргументов передавая в них только часть этих аргументов, результатом такого вызова будет другая функция от оставшихся аргументов.
     Каррирование ≠ частичное применение функции. Отличие: Каррирование принимает по 1 аргументу, частичное применение функции принимает > 1 аргумента.
     
     Плюсы над обычными указателями на функции (function pointer):
     - умеет захватывать переменные контекста [this, &].
     - может работатать как с lambda-функциями, так и с обычными функциями.
     - есть связывание с помощью std::bind: можно хранить копии/ссылки/указатели объектов классов, члены классов, аргументы к функциям/методам. С помощью placeholder можно подставлять значения при вызове function.
     - может рекурсивно вызывать самого себя.
     
     Минусы над обычными указателями на функции (function pointer):
     - является более затратным по ресурсам, т.к. требуется выделение памяти в куче.
     ОТЛИЧИЕ Lambda от function:
     - имеют разные типы.
     - function захватывает функтор/lambda, стирая ее тип: lambda хранится в куче как указатель void* привиденный с помощью reinterpret_cast к типу char[], а сам тип хранится отдельно в шаблонных параметрах.
     - в function выделение памяти происходит в куче, в lambda - на стеке, поэтому function теряет в производительности.
     - function может вызывать самого себя рекурсивно.
     */
    {
        FUNCTION::Start();
    }
    
    return 0;
}

