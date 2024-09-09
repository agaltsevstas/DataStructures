#include <iostream>
#include <functional>


/*
 Сайты: https://stackoverflow.com/questions/31623862/what-can-stdremove-extent-be-used-for
        https://stackoverflow.com/questions/13061979/can-you-make-a-stdshared-ptr-manage-an-array-allocated-with-new-t
 */

/*
 Unique_Ptr (сильный указатель - владеет ресурсом): 1 указатель может ссылаться только на один объект, конструктор копирования = delete, оператор копирования = delete. При вызове деструктора удаляет объект.
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
                delete ptr; // для указателей, которые nullptr/NULL, delete не приводит к abort/terminate
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
                delete[] ptr; // для указателей, которые nullptr/NULL, delete не приводит к abort/terminate
                // ptr = nullptr; // локальная переменная, nullptr не запомнится
            // }
        }
    };


    // deleter должен быть частью типа unique_ptr
    template <class TClass, typename Deleter = Default_Deleter<TClass>>
    class Unique_Ptr
    {
        using element_type = std::remove_extent_t<TClass>; // C++17: Для поддержки использования типа TClass[] - Unique_Ptr<int[]> mass(new int[10]);
        
        /// Конструктор копирования
        explicit Unique_Ptr(const Unique_Ptr& other) noexcept = delete;
        /// Оператор копирования
        [[nodiscard]] Unique_Ptr& operator=(const Unique_Ptr& other) = delete;
    public:
        /// Конструктор по-умолчанию
        Unique_Ptr() noexcept;
        Unique_Ptr(decltype(nullptr)) noexcept;
        explicit Unique_Ptr(element_type* iObject, Deleter deleter = Deleter()) noexcept;
        /// Конструктор перемещеиня
        explicit Unique_Ptr(Unique_Ptr&& other) noexcept;
        /// Деструктор
        ~Unique_Ptr() noexcept;
        /// Оператор перемещения
        Unique_Ptr& operator=(Unique_Ptr&& other) noexcept;
        Unique_Ptr& operator=(decltype(nullptr));
        auto operator<=>(const Unique_Ptr&) const = default; // сравнение по-умолчанию
        bool operator==(Unique_Ptr&& other); // Особый случай
        element_type* Get() noexcept;
        const element_type* Get() const noexcept;
        Deleter& Get_Deleter();
        element_type& operator*() noexcept;
        element_type* operator->() noexcept;
        // operator const void*() const noexcept; // object == nullptr
        [[nodiscard]] element_type& operator[](uint64_t i); // C++17: можно обращаться к элементам массива
        void Swap(Unique_Ptr& other);
        void Reset();
        
    private:
        element_type* _ptr = nullptr;
        Deleter _deleter;
    };

    template <>
    class Unique_Ptr<void> // Запрет на создание void
    {
        Unique_Ptr() = delete; // Удаление конструктора
    };

    /// Конструктор по-умолчанию
    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>::Unique_Ptr() noexcept : _ptr(nullptr)
    {
        std::cout << "Constructor nullptr" << std::endl;
    }

    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>::Unique_Ptr(element_type* ptr, Deleter deleter) noexcept :
    _ptr(ptr),
    _deleter(deleter)
    {
        std::cout << "Constructor new" << std::endl;
    }

    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>::Unique_Ptr(decltype(nullptr)) noexcept
    {
        Unique_Ptr();
    }

    /// Конструктор перемещения
    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>::Unique_Ptr(Unique_Ptr&& other) noexcept
    {
        std::cout << "Move constructor: ";
        *this = std::move(other);
    }

    /// Деструктор
    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>::~Unique_Ptr() noexcept
    {
        std::cout << "Destructor: delete object" << std::endl;
        _deleter(_ptr);
    }

    /// Оператор перемещения
    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>& Unique_Ptr<TClass, Deleter>::operator=(Unique_Ptr&& other) noexcept
    {
        if (this == &other) // object = std::move(object)
            return *this;
        
        if (!other._ptr)
            throw "_ptr - nullptr!";
        
        _deleter(_ptr);
        _ptr = std::exchange(other._ptr, nullptr); // std::move не работает с сырыми указателями
        _deleter = std::move(other._deleter);
        std::cout << "operator= delete object and move" << std::endl;
        return *this;
    }

    template <class TClass, typename Deleter>
    Unique_Ptr<TClass, Deleter>& Unique_Ptr<TClass, Deleter>::operator=(decltype(nullptr))
    {
        _deleter(_ptr);
        _ptr = nullptr;
        std::cout << "delete object" << std::endl;
        return *this;
    }

    template <class TClass, typename Deleter>
    bool Unique_Ptr<TClass, Deleter>::operator==(Unique_Ptr&& other)
    {
        return _ptr == other._ptr; // сравниваем адреса указателей
    }

    template <class TClass, typename Deleter>
    std::remove_extent_t<TClass>* Unique_Ptr<TClass, Deleter>::Get() noexcept
    {
        return _ptr ? _ptr : nullptr;
    }

    template <class TClass, typename Deleter>
    const std::remove_extent_t<TClass>* Unique_Ptr<TClass, Deleter>::Get() const noexcept
    {
        return _ptr ? _ptr : nullptr;
    }

    template <class TClass, typename Deleter>
    std::remove_extent_t<TClass>& Unique_Ptr<TClass, Deleter>::operator*() noexcept
    {
        return *Get();
    }

    template <class TClass, typename Deleter>
    std::remove_extent_t<TClass>* Unique_Ptr<TClass, Deleter>::operator->() noexcept
    {
        return Get();
    }

    /*
     template <class TClass, typename Deleter>
     Unique_Ptr<TClass, Deleter>::operator const void*() const noexcept
     {
         return Get(); // object == nullptr
     }
     */

    template <class TClass, typename Deleter>
    Deleter& Unique_Ptr<TClass, Deleter>::Get_Deleter()
    {
        return _deleter;
    }

    /// C++17: можно обращаться к элементам массива
    template <class TClass, typename Deleter>
    std::remove_extent_t<TClass>& Unique_Ptr<TClass, Deleter>::operator[](uint64_t i)
    {
        return *Get();
    }

    template <class TClass, typename Deleter>
    void Unique_Ptr<TClass, Deleter>::Swap(Unique_Ptr& other)
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_ptr, other._ptr);
        std::swap(_deleter, other._deleter);
    }

    // Reset - менее безопасный чем Make_Shared
    template <class TClass, typename Deleter>
    void Unique_Ptr<TClass, Deleter>::Reset()
    {
        std::cout << "delete object" << std::endl;
        _deleter(_ptr); // почему-то при выходе nullptr отбрасывает
        _ptr = nullptr;
    }

    /*
     make_unique - функция, не требующие дублирования типа (auto ptr = make_unique<int>(10)). Стоит использовать make_unique вместо unique_ptr<T>(new T()).
     Плюсы:
     - не нужно писать new.
     - там не нужно дублировать тип unique_ptr<int> number(new int(1)) -> auto number = make_unique<int>(1).
     Минусы:
     - не может использовать deleter.
     - перегруженные operator new и operator delete в классе будут проигнорированы в make_unique.
     */
    // Make_Shared - более безопасный чем Shared_Ptr::Reset
    template <class TClass, typename ...TArgs>
    inline Unique_Ptr<TClass> Make_Unique(TArgs&& ...iArgs)
    {
        // std::allocate_unique<TClass>(allocator<TClass>(), std::forward<_Args>(iArgs)...);
        using element_type = std::remove_extent_t<TClass>; // C++17: Для поддержки использования типа Make_Unique<int[]>(10);
        return Unique_Ptr<TClass>(new element_type(std::forward<TArgs>(iArgs)...));
    }
}

struct A
{
    ~A()
    {
        std::cout << "~A()" << std::endl;
    }
};

struct B : A
{
    ~B()
    {
        std::cout << "~B()" << std::endl;
    }
};

decltype(auto) Exception(const auto& value) // decltype(auto) - не отбрасывает ссылки и возвращает lvalue, иначе rvalue
{
    // throw "Error!";
    return value;
}

template <class T>
void function(const STD::Unique_Ptr<T>& ptr, const auto& value)
{
    
}


int main()
{
    using namespace STD;
    
    /* Деструктор struct A - невиртуальный, поэтому должно вызваться только деструктор: ~A() - это неверно. std::shared_ptr хранит внутри себя deleter, который знает тип объекта, переданного в конструктор, и поэтому будут вызываться все деструкторы:
     ~B()
     ~A()
     */
    
    std::shared_ptr<A> base_ptr1 = std::make_shared<B>();
    
    /*
     Если заменить std::shared_ptr на Unique_Ptr, то в Unique_Ptr deleter является частью типа, поэтому будет вызываться только деструктор:
     ~A()
     */
    
    /// TODO: Unique_Ptr<A> base_ptr2 = Make_Unique<B>();
    
    /*
     До C++17, После C++17 вроде как проблема решена
     Компилятор может оптимизировать порядок выполнения:
     1) Выделить память new
     2) Вызвать Exception -> может вылететь исключение
     3) Если вылетело исключение в конструктор уже не зайдет -> утечка памяти
     */
    function(Unique_Ptr<int>(new int(10)), Exception(1));
    
    // Constructor, Move, Swap, Reset
    {
        // Unique_Ptr<void> void_ptr((void*)new int); // Запрет на создание void, конструктор = delete
        Unique_Ptr<int> number_null;
        int* value = new int(5);
        Unique_Ptr<int> number(value); // Bad practice: класть в конструктор сырой указатель, но в отличие от std::shared_ptr двойного delete не будет
        Unique_Ptr<int> number1(new int(1)); // Good practise: выделять в конструктор динамическую память
        auto number2 = Make_Unique<int>(2);
        auto number1_ptr = number1.Get();
        auto number2_ptr = number2.Get();
        
        number1.Swap(number2);
        number1 = std::move(number1);
        number1 = std::move(number2);
        number1_ptr = number1.Get();
        number2_ptr = number2.Get();
        
        Unique_Ptr<int> number3(std::move(number1));
        *number3 = 3;
        number1_ptr = number1.Get();
        number2_ptr = number2.Get();
        auto number3_ptr = number2.Get();
        
        number3.Reset();
        number3_ptr = number2.Get();
        
        // Unique_Ptr<int> mass(new int[10]); // Вызовется Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Unique_Ptr<int[]> mass(new int[10]); // C++17: Вызовется правильный deleter
        [[maybe_unused]] auto& mass_index_0 = mass[(uint64_t)0] = 5; // 1 элемент массива = 5
        Unique_Ptr<int[]> mass_unique = Make_Unique<int[]>(10);
    }
    
    // Deleter
    {
        auto deleter = [](int* ptr)
        {
            delete ptr;
        };
        
        Unique_Ptr<int, decltype(deleter)> number1(new int(1), deleter);
        Unique_Ptr<int, decltype(number1.Get_Deleter())> number2(new int(2), number1.Get_Deleter());
        Unique_Ptr<int, Default_Deleter<int>> number3(new int(3)); // Вызовется functor deleter
        Unique_Ptr<int, Default_Deleter<int[]>> mass(new int[10]); // Без указания Default_Deleter<int[]> выберет Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Unique_Ptr<int[], Default_Deleter<int[]>> mass2(new int[10]); // C++17
    }
    
    return 0;
}
