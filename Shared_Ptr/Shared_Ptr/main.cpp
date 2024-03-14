#include <iostream>
#include <functional>

#include "Shared_Ptr.h"


/*
 Сайты: https://stackoverflow.com/questions/31623862/what-can-stdremove-extent-be-used-for
        https://stackoverflow.com/questions/13061979/can-you-make-a-stdshared-ptr-manage-an-array-allocated-with-new-t
        https://github.com/google/shipshape/blob/master/third_party/proto/src/google/protobuf/stubs/shared_ptr.h
 */


auto Exception(const auto& value)
{
    // throw "Error!";
    return value;
}

template <class T>
void function(const STD::Shared_Ptr<T>& ptr, const auto& value)
{
    
}


int main()
{
    using namespace STD;
    
    /*
     До C++17, После C++17 вроде как проблема решена
     Компилятор может оптимизировать порядок выполнения:
     1) Выделить память new
     2) Вызвать Exception -> может вылететь исключение
     3) Если вылетело исключение в конструктор уже не зайдет -> утечка памяти
     */
    function(Shared_Ptr<int>(new int(10)), Exception(1));
    
    // Constructor, Move, Swap, Reset
    {
         // Shared_Ptr<void> void_ptr((void*)new int); // Запрет на создание void, конструктор = delete
        Shared_Ptr<int> number_null;
        int* value = new int(5);
        Shared_Ptr<int> number(value); // Bad practice: класть в конструктор сырой указатель, возможен двойной delete
        Shared_Ptr<int> number1(new int(1)); // Good practise: выделять в конструктор динамическую память
        auto number2 = Make_Shared<int>(2);
        auto number1_ptr = number1.Get();
        auto number2_ptr = number2.Get();
        
        // Move
        number1.Swap(number2);
        number1 = std::move(number1);
        number1 = std::move(number2);
        number1_ptr = number1.Get();
        number2_ptr = number2.Get();
        
        Shared_Ptr<int> number3(std::move(number1));
        *number3 = 3;
        auto number3_count = number3.Use_Count();
        bool number3_unique = number3.Unique();
        number3 = number3;
        number3_count = number3.Use_Count();
        number3_unique = number3.Unique();
        
        // Copy
        Shared_Ptr<int> number4(number3);
        number3_count = number3.Use_Count();
        number3_unique = number3.Unique();
        auto number5 = number3;
        number3_count = number3.Use_Count();
        number3_unique = number3.Unique();
        number3.Reset();
        number3_count = number3.Use_Count();
        number3_unique = number3.Unique();
        number4.Reset(new int(4));
        number3_count = number3.Use_Count();
        number3_unique = number3.Unique();
        auto number6 = number4;
        auto number7 = std::move(number6);
        number4.Reset();
        auto number4_count = number4.Use_Count();
        bool number4_unique = number4.Unique();
        number4.Reset();
        number4_count = number4.Use_Count();
        number4_unique = number4.Unique();
        number4.Reset();
        
        // Shared_Ptr<int> mass(new int[10]); // Вызовется Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Shared_Ptr<int[]> mass(new int[10]); // C++17: Вызовется правильный deleter
        [[maybe_unused]] auto& mass_index_0 = mass[(uint64_t)0] = 5; // 1 элемент массива = 5
    }
    
    // Deleter, иногда XCode выдает ошибку: malloc: Heap corruption detected, free list is damaged
    {
        auto deleter = [](int* ptr)
        {
            delete ptr;
        };
        
        Shared_Ptr<int> number1(new int(1), deleter);
        Shared_Ptr<int> number(new int(2), [](int *ptr) { delete ptr; ptr = nullptr; }); // lambda deleter
        Shared_Ptr<int> mass1(new int[10], Default_Deleter<int[]>()); // Без указания Default_Deleter<int[]> выберет Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Shared_Ptr<int> mass2(new int[10], [](int *ptr) { delete[] ptr; ptr = nullptr; });
        Shared_Ptr<int[]> mass(new int[10], [](int *ptr) { delete[] ptr; ptr = nullptr; }); // C++17
    }
    
    return 0;
}

