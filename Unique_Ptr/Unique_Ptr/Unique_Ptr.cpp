#include <iostream>
#include <functional>


/*
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


    // deleter должен быть частью типа unique_ptr
    template <class TClass, typename Deleter = Default_Deleter<TClass>>
    class Unique_Ptr
    {
        using element_type = std::remove_extent_t<TClass>; // C++17: Для подддержки использования типа TClass[] - Unique_Ptr<int[]> mass_deleter2(new int[10]);
        
        /// Конструктор копирования
        explicit Unique_Ptr(const Unique_Ptr& other) noexcept = delete;
        /// Оператор копирования
        [[nodiscard]] Unique_Ptr& operator=(const Unique_Ptr& other) = delete;
    public:
        /// Конструктор по-умолчанию
        Unique_Ptr() noexcept;
        explicit Unique_Ptr(element_type* iObject, Deleter deleter = Deleter()) noexcept;
        /// Конструктор перемещеиня
        explicit Unique_Ptr(Unique_Ptr&& other) noexcept;
        /// Деструктор
        ~Unique_Ptr() noexcept;
        /// Оператор перемещения
        Unique_Ptr& operator=(Unique_Ptr&& other);
        auto operator<=>(const Unique_Ptr&) const = default; // сравнение по-умолчанию
        bool operator==(Unique_Ptr&& other); // Особый случай
        [[nodiscard("Get")]] element_type* Get() noexcept;
        [[nodiscard("Get")]] const element_type* Get() const noexcept;
        [[nodiscard("Get_Deleter")]] Deleter& Get_Deleter();
        [[nodiscard("*")]] element_type& operator*() const noexcept;
        element_type& operator[](uint64_t i); // C++17: можно обращаться к элементам массива
        void Swap(Unique_Ptr& other);
        
        // Reset - менее безопасный чем Make_Shared
        void Reset(TClass* ptr = nullptr);
        
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
    Unique_Ptr<TClass, Deleter>& Unique_Ptr<TClass, Deleter>::operator=(Unique_Ptr&& other)
    {
        if (this == &other) // object = std::move(object)
            return *this;
        
        if (!other._ptr)
            throw "_ptr - nullptr!";
        
        _deleter(_ptr);
        _ptr = std::exchange(other._ptr, nullptr); // std::move не работает с сырыми указателями
        std::cout << "operator= delete object and move" << std::endl;
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
    Deleter& Unique_Ptr<TClass, Deleter>::Get_Deleter()
    {
        return _deleter;
    }

    template <class TClass, typename Deleter>
    std::remove_extent_t<TClass>& Unique_Ptr<TClass, Deleter>::operator*() const noexcept
    {
        return *Get();
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
        std::swap(_ptr, other._ptr);
    }

    // Reset - менее безопасный чем Make_Shared
    template <class TClass, typename Deleter>
    void Unique_Ptr<TClass, Deleter>::Reset(TClass* ptr)
    {
        deleter(_ptr);
        std::cout << "delete object" << std::endl;
        
        if (ptr)
        {
            _ptr = ptr;
            std::cout << "reset object" << std::endl;
        }
    }

    // Make_Shared - более безопасный чем Shared_Ptr::Reset
    template <class TClass, typename ...TArgs>
    Unique_Ptr<TClass> Make_Unique(TArgs&& ...iArgs)
    {
//        std::allocate_unique<TClass>(allocator<TClass>(), std::forward<_Args>(iArgs)...);
        return Unique_Ptr<TClass>(new TClass(std::forward<TArgs>(iArgs)...));
    }
}

auto Exception(const auto& value)
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
    
    /*
     До C++17, После C++17 вроде как проблема решена
     Компилятор может оптимизировать порядок выполнения:
     1) Выделить память new
     2) Вызвать Exception -> может вылететь исключение
     3) Если вылетело исключение в конструктор уже не зайдет -> утечка памяти
     */
    function(Unique_Ptr<int>(new int(10)), Exception(1));
    
    // Constructor, move, swap
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
        number1_ptr = number1.Get();
        number2_ptr = number2.Get();
        
        // Unique_Ptr<int> mass(new int[10]); // Вызовется Default_Deleter<int> по-умолчанию и будет утечка памяти для 9 элементов
        Unique_Ptr<int[]> mass(new int[10]); // C++17: Вызовется правильный deleter
        [[maybe_unused]] auto& mass_index_0 = mass[(uint64_t)0] = 5; // 1 элемент массива = 5
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
    }
    
    return 0;
}
