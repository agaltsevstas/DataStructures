#include <iostream>
#include <functional>


/*
 Лекция: https://www.youtube.com/watch?v=9ZSBOfTd-sc&ab_channel=%D0%9C%D0%B5%D1%89%D0%B5%D1%80%D0%B8%D0%BD%D0%98%D0%BB%D1%8C%D1%8F
 Сайты: https://stackoverflow.com/questions/31623862/what-can-stdremove-extent-be-used-for
        https://stackoverflow.com/questions/13061979/can-you-make-a-stdshared-ptr-manage-an-array-allocated-with-new-t
 */

namespace STD
{
    template <class T>
    struct Default_Deleter
    {
        void operator()(T* ptr)
        {
            // if (ptr)
            // {
                delete ptr; // для указателей, которые nullptr/NULL, delete не приводит к abort/terminate.
                // ptr = nullptr; // локальная переменная, nullptr не запомнится
            // }
        }
    };

    template <class T>
    struct Default_Deleter<T[]> // C++17: explicit (или full) specialization пишется уже после основного
    {
        void operator()(T* ptr)
        {
            // if (ptr)
            // {
                delete[] ptr; // для указателей, которые nullptr/NULL, delete не приводит к abort/terminate.
                // ptr = nullptr; // локальная переменная, nullptr не запомнится
            // }
        }
    };

/*
 Shared_Ptr (сильный указатель - владеет ресурсом): производит два раздельных выделения памяти: аллокация управляемого объекта + Control Block(atomic shared_count (сильные ссылки), atomic weak_count (слабые ссылки), allocator, deleter), вместо одновременного выделения памяти make_shared. При вызове конструктора копирования или оператора копирования ++shared_count, при вызове коструктора перемещения, оператора перемещения или деструктора --shared_count. При shared_count == 0 удаляет объект + weak_count == 0 удаляет Control Block.
 
 Передавать Shared_Ptr в качестве аргумента функции/метода/конструктора следует по const ссылке, по значению передавать только тогда, когда речь идет о работе в отдельном потоке (чтобы программист видел и имел это в виду).
 Характеристики:
 + не нужно делать лишнюю итерацию на увеличение shared_count (сильные ссылки) в Control Block.
 + право собственности на объект имеют только классы/структуры и функции, работающие в многопотчном режиме, ну и место на стеке, где они создавались.
 */

    template <class TClass>
    class Custom_Shared_Ptr
    {
        using element_type = std::remove_extent_t<TClass>; // C++17: Для поддержки использования типа T[] - Unique_Ptr<int[]> mass(new int[10]);
    public:
        /// Конструктор по-умолчанию
        Custom_Shared_Ptr() noexcept;
        Custom_Shared_Ptr(decltype(nullptr)) noexcept;
        template <typename Deleter = Default_Deleter<element_type>>
        explicit Custom_Shared_Ptr(element_type* iObject, Deleter deleter = Deleter());
        /// Конструктор копирования
        Custom_Shared_Ptr(const Custom_Shared_Ptr& other);
        /// Конструктор перемещеиня
        Custom_Shared_Ptr(Custom_Shared_Ptr&& other) noexcept;
        /// Деструктор
        ~Custom_Shared_Ptr();
        /// Оператор копирования
        Custom_Shared_Ptr& operator=(const Custom_Shared_Ptr& other);
        /// Оператор перемещения
        Custom_Shared_Ptr& operator=(Custom_Shared_Ptr&& other) noexcept;
        Custom_Shared_Ptr& operator=(decltype(nullptr));
        auto operator<=>(const Custom_Shared_Ptr&) const = default; // сравнение по-умолчанию
        bool operator==(const Custom_Shared_Ptr& other); // Особый случай
        element_type* Get() noexcept;
        const element_type* Get() const noexcept;
        element_type& operator*() noexcept;
        element_type* operator->() noexcept;
        // operator const void*() const noexcept; // object == nullptr
        [[nodiscard]] element_type& operator[](uint64_t i); // C++17: можно обращаться к элементам массива
        void Swap(Custom_Shared_Ptr& other);
        [[nodiscard("Unique")]] bool Unique() const noexcept;
        [[nodiscard("Use_Count")]] uint64_t Use_Count() const noexcept;
        
        // Reset - менее безопасный чем Make_Shared
        void Reset(TClass* ptr = nullptr);
        
    private:
        element_type* _ptr = nullptr;
        uint64_t* _count = nullptr;
        std::function<void(element_type*)> _deleter;
    };

    template <>
    class Custom_Shared_Ptr<void> // Запрет на создание void
    {
        Custom_Shared_Ptr() = delete; // Удаление конструктора
    };

    /// Конструктор по-умолчанию
    template <class TClass>
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr() noexcept : _ptr(nullptr), _count(nullptr)
    {
        std::cout << "Constructor nullptr" << std::endl;
    }

    template <class TClass>
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr(decltype(nullptr)) noexcept
    {
        Custom_Shared_Ptr();
    }

    template <class TClass>
    template <typename Deleter>
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr(element_type* ptr, Deleter deleter) :
    _ptr(ptr),
    _count(ptr ? new uint64_t(1) : nullptr),
    _deleter(deleter)
    {
        std::cout << "Constructor new" << std::endl;
    }

    /// Конструктор копирования
    template <class TClass>
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr(const Custom_Shared_Ptr& other)
    {
        std::cout << "Сopy constructor: ";
        *this = other;
    }

    /// Конструктор перемещения
    template <class TClass>
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr(Custom_Shared_Ptr&& other) noexcept
    {
        std::cout << "Move constructor: ";
        *this = std::move(other);
    }

    /// Деструктор
    template <class TClass>
    Custom_Shared_Ptr<TClass>::~Custom_Shared_Ptr()
    {
        if (_count)
        {
            std::cout << "Destructor: ";
            if (--(*_count) == 0)
            {
                _deleter(_ptr);
                _ptr = nullptr;
                delete _count;
                _count = nullptr;
                std::cout << "delete object" << std::endl;
            }
            else
            {
                /*
                 Следующее поведение нормально!
                 int* ptr1 = new int(1);
                 int* ptr2 = ptr1;
                 delete ptr1;
                 ptr1 = nullptr; // ptr2 будет не nullptr
                 */
                std::cout << "decrease count - " << *_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
    }

    /// Оператор копирования
    template <class TClass>
    Custom_Shared_Ptr<TClass>& Custom_Shared_Ptr<TClass>::operator=(const Custom_Shared_Ptr& other)
    {
        if (this == &other) // object = object
            return *this;
        
        if (_count) // Custom_Shared_Ptr<T> object1(object2);
        {
            if (--(*_count) == 0)  // object1 = std::move(object2)
            {
                _deleter(_ptr);
                delete _count;
                std::cout << "delete object" << std::endl;
            }
            else
                std::cout << "decrease count - " << *_count << std::endl;
        }
        
        _ptr = other._ptr;
        _count = other._count;
        _deleter = other._deleter;
        ++(*_count);
        std::cout << "operator=: increase count - " << *_count << std::endl;
        return *this;
    }

    /// Оператор перемещения
    template <class TClass>
    Custom_Shared_Ptr<TClass>& Custom_Shared_Ptr<TClass>::operator=(Custom_Shared_Ptr&& other) noexcept
    {
        if (this == &other) // object = std::move(object)
            return *this;
        
        if (_count) // Custom_Shared_Ptr<T> object1(std::move(object2));
        {
            if (--(*_count) == 0)  // object1 = std::move(object2)
            {
                _deleter(_ptr);
                delete _count;
                std::cout << "delete object" << std::endl;
            }
            else
                std::cout << "decrease count - " << *_count << std::endl;
        }
        
        _ptr = std::exchange(other._ptr, nullptr); // std::move не работает с сырыми указателями
        _count = std::exchange(other._count, nullptr); // std::move не работает с сырыми указателями
        _deleter = std::move(other._deleter); // std::move не работает с сырыми указателями
        std::cout << "operator=" << std::endl;
        return *this;
    }

    template <class TClass>
    Custom_Shared_Ptr<TClass>& Custom_Shared_Ptr<TClass>::operator=(decltype(nullptr))
    {
        if (_count) // Custom_Shared_Ptr<T> object1 = nullptr;
        {
            if (--(*_count) == 0)  // object1 = nullptr
            {
                _deleter(_ptr);
                delete _count;
                std::cout << "delete object" << std::endl;
            }
            else
                std::cout << "decrease count - " << *_count << std::endl;
        }
        return *this;
    }

    template <class TClass>
    bool Custom_Shared_Ptr<TClass>::operator==(const Custom_Shared_Ptr& other)
    {
        return _ptr == other._ptr && _count == other._count; // сравниваем адреса указателей
    }

    template <class TClass>
    std::remove_extent_t<TClass>* Custom_Shared_Ptr<TClass>::Get() noexcept
    {
        return _ptr ? _ptr : nullptr;
    }

    template <class TClass>
    const std::remove_extent_t<TClass>* Custom_Shared_Ptr<TClass>::Get() const noexcept
    {
        return _ptr ? _ptr : nullptr;
    }

    template <class TClass>
    std::remove_extent_t<TClass>& Custom_Shared_Ptr<TClass>::operator*() noexcept
    {
        return *Get();
    }

    template <class TClass>
    std::remove_extent_t<TClass>* Custom_Shared_Ptr<TClass>::operator->() noexcept
    {
        return Get();
    }

    /*
     template <class TClass>
     Custom_Shared_Ptr<TClass>::operator const void*() const noexcept
     {
         return Get(); // object == nullptr
     }
     */

    /// C++17: можно обращаться к элементам массива
    template <class TClass>
    std::remove_extent_t<TClass>& Custom_Shared_Ptr<TClass>::operator[](uint64_t i)
    {
        return *Get();
    }

    template <class TClass>
    void Custom_Shared_Ptr<TClass>::Swap(Custom_Shared_Ptr& other)
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_ptr, other._ptr);
        std::swap(_count, other._count);
    }

    template <class TClass>
    bool Custom_Shared_Ptr<TClass>::Unique() const noexcept
    {
        return _count ? (*_count) == 1 : false;
    }

    template <class TClass>
    uint64_t Custom_Shared_Ptr<TClass>::Use_Count() const noexcept
    {
        return _count ? *_count : 0;
    }

    // Reset - менее безопасный чем Make_Shared
    template <class TClass>
    void Custom_Shared_Ptr<TClass>::Reset(TClass* ptr)
    {
        if (_count)
        {
            if (--(*_count) == 0)
            {
                _deleter(_ptr);
                _ptr = nullptr;
                delete _count;
                _count = nullptr;
                std::cout << "delete object" << std::endl;
            }
            else
            {
                /*
                 Следующее поведение нормально!
                 int* ptr1 = new int(1);
                 int* ptr2 = ptr1;
                 delete ptr1;
                 ptr1 = nullptr; // ptr2 будет не nullptr
                 */
                std::cout << "decrease count - " << *_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
        
        if (ptr)
        {
            _ptr = ptr;
            _count = new uint64_t(1);
            _deleter = Default_Deleter<TClass>();
            std::cout << "new object" << std::endl;
        }
    }

    /*
     make_shared - функция, не требующая дублирования типа (auto ptr = make_shared<int>(10)). Стоит использовать make_shared вместо shared_ptr<T>(new T()). make_shared - нужна для повышения производительности по сравнению shared_ptr(new), которая с помощью вызова конструктора требуют минимум две аллокации: одну — для размещения объекта, вторую — для Control Block.
     Плюсы:
     - невозможна утечка памяти.
     - не нужно писать new.
     - там не нужно дублировать тип shared<int> number(new int(1)) -> auto number = make_shared<int>(1).
     - make_shared - производит одно выделение памяти вместе: аллокация управляемого объекта + Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter), следовательно они будут находиться в одном участке памяти и работать с ними быстрее, по сравнению с раздельным выделением памяти в shared_ptr.
     Минусы:
     - не может использовать deleter.
     - перегруженные operator new и operator delete в классе будут проигнорированы в make_shared.
     - make_shared не может вызывать private конструкторы внутри метода (например, синглтон).
     - для make_shared нужно ждать shared_count == weak_count == 0, чтобы удалить объект + Control Block.
     */
    // Make_Shared - более безопасный чем Shared_Ptr::Reset
    template <class TClass, typename ...TArgs>
    Custom_Shared_Ptr<TClass> Make_Shared(TArgs&& ...iArgs)
    {
        // std::allocate_shared<TClass>(allocator<TClass>(), std::forward<_Args>(iArgs)...);
        using element_type = std::remove_extent_t<TClass>; // C++17: Для поддержки использования типа Make_Shared<int[]>(10);
        return Custom_Shared_Ptr<TClass>(new element_type(std::forward<TArgs>(iArgs)...));
    }
}

decltype(auto) Exception(const auto& value) // decltype(auto) - не отбрасывает ссылки и возвращает lvalue, иначе rvalue
{
    // throw "Error!";
    return value;
}

template <class T>
void function(const STD::Custom_Shared_Ptr<T>& ptr, const auto& value)
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
    function(Custom_Shared_Ptr<int>(new int(10)), Exception(1));
    
    // Constructor, Move, Swap, Reset
    {
         // Custom_Shared_Ptr<void> void_ptr((void*)new int); // Запрет на создание void, конструктор = delete
        Custom_Shared_Ptr<int> number_null = nullptr;
        number_null = nullptr;
        int* value = new int(5);
        Custom_Shared_Ptr<int> number(value); // Bad practice: класть в конструктор сырой указатель, возможен двойной delete
        Custom_Shared_Ptr<int> number1(new int(1)); // Good practise: выделять в конструктор динамическую память
        auto number2 = Make_Shared<int>(2);
        auto number1_ptr = number1.Get();
        auto number2_ptr = number2.Get();
        
        // Move
        number1.Swap(number2);
        number1 = std::move(number1);
        number1 = std::move(number2);
        number1_ptr = number1.Get();
        number2_ptr = number2.Get();
        
        Custom_Shared_Ptr<int> number3(std::move(number1));
        *number3 = 3;
        auto number3_count = number3.Use_Count();
        bool number3_unique = number3.Unique();
        number3 = number3;
        number3_count = number3.Use_Count();
        number3_unique = number3.Unique();
        
        // Copy
        Custom_Shared_Ptr<int> number4(number3);
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
        
        // Custom_Shared_Ptr<int> mass(new int[10]); // Вызовется Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Custom_Shared_Ptr<int[]> mass(new int[10]); // C++17: Вызовется правильный deleter
        [[maybe_unused]] auto& mass_index_0 = mass[(uint64_t)0] = 5; // 1 элемент массива = 5
        [[maybe_unused]] Custom_Shared_Ptr<int[]> mass_shared = Make_Shared<int[]>(10);
    }
    
    // Deleter, иногда XCode выдает ошибку: malloc: Heap corruption detected, free list is damaged
    {
        auto deleter = [](int* ptr)
        {
            delete ptr;
        };
        
        Custom_Shared_Ptr<int> number1(new int(1), deleter);
        Custom_Shared_Ptr<int> number(new int(2), [](int *ptr) { delete ptr; ptr = nullptr; }); // lambda deleter
        Custom_Shared_Ptr<int> mass1(new int[10], Default_Deleter<int[]>()); // Без указания Default_Deleter<int[]> выберет Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Custom_Shared_Ptr<int> mass2(new int[10], [](int *ptr) { delete[] ptr; ptr = nullptr; });
        Custom_Shared_Ptr<int[]> mass(new int[10], [](int *ptr) { delete[] ptr; ptr = nullptr; }); // C++17
    }
    
    return 0;
}
