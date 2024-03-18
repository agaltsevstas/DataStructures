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
            if (ptr)
            {
                delete ptr;
                ptr = nullptr;
            }
        }
    };

    template <class T>
    struct Default_Deleter<T[]> // C++17: explicit (или full) specialization пишется уже после основного
    {
        void operator()(T* ptr)
        {
            if (ptr)
            {
                delete[] ptr;
                ptr = nullptr;
            }
        }
    };


    template <class TClass>
    class Custom_Shared_Ptr
    {
        using element_type = std::remove_extent_t<TClass>; // C++17: Для подддержки использования типа T[] - Unique_Ptr<int[]> mass(new int[10]);
    public:
        /// Конструктор по-умолчанию
        Custom_Shared_Ptr() noexcept;
        Custom_Shared_Ptr(decltype(nullptr)) noexcept;
        template <typename Deleter = Default_Deleter<element_type>>
        explicit Custom_Shared_Ptr(element_type* iObject, Deleter deleter = Deleter()) noexcept;
        /// Конструктор копирования
        Custom_Shared_Ptr(const Custom_Shared_Ptr& other) noexcept;
        /// Конструктор перемещеиня
        Custom_Shared_Ptr(Custom_Shared_Ptr&& other) noexcept;
        /// Деструктор
        ~Custom_Shared_Ptr() noexcept;
        /// Оператор копирования
        Custom_Shared_Ptr& operator=(const Custom_Shared_Ptr& other);
        /// Оператор перемещения
        Custom_Shared_Ptr& operator=(Custom_Shared_Ptr&& other);
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
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr(element_type* ptr, Deleter deleter) noexcept :
    _ptr(ptr),
    _count(ptr ? new uint64_t(1) : nullptr),
    _deleter(deleter)
    {
        std::cout << "Constructor new" << std::endl;
    }

    /// Конструктор копирования
    template <class TClass>
    Custom_Shared_Ptr<TClass>::Custom_Shared_Ptr(const Custom_Shared_Ptr& other) noexcept
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
    Custom_Shared_Ptr<TClass>::~Custom_Shared_Ptr() noexcept
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
    Custom_Shared_Ptr<TClass>& Custom_Shared_Ptr<TClass>::operator=(Custom_Shared_Ptr&& other)
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

    // Make_Shared - более безопасный чем Shared_Ptr::Reset
    template <class TClass, typename ...TArgs>
    Custom_Shared_Ptr<TClass> Make_Shared(TArgs&& ...iArgs)
    {
        // std::allocate_shared<TClass>(allocator<TClass>(), std::forward<_Args>(iArgs)...);
        return Custom_Shared_Ptr<TClass>(new TClass(std::forward<TArgs>(iArgs)...));
    }
}

auto Exception(const auto& value)
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
