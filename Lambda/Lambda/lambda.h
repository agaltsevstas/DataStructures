#ifndef lambda_h
#define lambda_h

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
 
 ОТЛИЧИЕ Lambda от function:
 - имеют разные типы.
 - function захватывает lambda, стирая ее тип: lambda хранится в куче как todo: void* указатель, а тип хранится отдельно.
 - в function выделение памяти происходит в куче, в lambda - на стеке, поэтому function теряет в производительности.
 - function может вызывать самого себя рекурсивно.
 */

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
 Сайты: https://rigtorp.se/iife/
        https://learn.microsoft.com/ru-ru/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170
        https://stackoverflow.com/questions/77186627/how-to-perform-complex-initialization-of-a-const-data-member-in-the-member-initi
        
 */

/*
 Проверка создании лямбды: https://cppinsights.io/
 */

namespace LAMBDA
{
    class Lambda
    {
    public:
        Lambda(const int& number1, const int& number2, int& number3, int& number4):
        _number1(number1),
        _number2(number2),
        _number3(number3),
        _number4(number4)
        {}
        
        template<class Type1, class Type2>
        inline bool operator()(Type1 lhs, Type2 rhs) const
        {
            // _number1 = 1; // Ошибка: Cannot assign to non-static data member '_number1' with const-qualified type 'const int'
            // _number2 = 2; // Ошибка: Cannot assign to non-static data member '_number2' with const-qualified type 'const int &'
            // _number3 = 3; // Ошибка: Cannot assign to non-static data member within const member function 'operator()'
            _number4 = 4;
            return lhs < rhs;
        }
        
    private:
        const int _number1;
        const int& _number2;
        int _number3;
        int& _number4;
    };
}
#endif /* lambda_h */
