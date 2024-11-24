#ifndef Shared_Ptr_h
#define Shared_Ptr_h

#include "ControlBlock.h"


/*
 Лекция: https://www.youtube.com/watch?v=9ZSBOfTd-sc&ab_channel=%D0%9C%D0%B5%D1%89%D0%B5%D1%80%D0%B8%D0%BD%D0%98%D0%BB%D1%8C%D1%8F
 Сайты: https://stackoverflow.com/questions/31623862/what-can-stdremove-extent-be-used-for
        https://stackoverflow.com/questions/13061979/can-you-make-a-stdshared-ptr-manage-an-array-allocated-with-new-t
        https://github.com/google/shipshape/blob/master/third_party/proto/src/google/protobuf/stubs/shared_ptr.h
        https://github.com/yalekseev/shared_ptr/blob/master/shared_ptr.h
 */

/*
 Shared_Ptr (сильный указатель - владеет ресурсом): производит два раздельных выделения памяти: аллокация управляемого объекта + Control Block(atomic shared_count (сильные ссылки), atomic weak_count (слабые ссылки), allocator, deleter), вместо одновременного выделения памяти make_shared. При вызове конструктора копирования или оператора копирования ++shared_count, при вызове коструктора перемещения, оператора перемещения или деструктора --shared_count. При shared_count == 0 удаляет объект + weak_count == 0 удаляет Control Block.
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


    template <class TClass>
    class Shared_Ptr
    {
        template <typename U> friend class Weak_Ptr;
        using element_type = std::remove_extent_t<TClass>; // C++17: Для поддержки использования типа T[] - Unique_Ptr<int[]> mass(new int[10]);
    public:
        /// Конструктор по-умолчанию
        Shared_Ptr() noexcept;
        Shared_Ptr(decltype(nullptr)) noexcept;
        template <typename Deleter = Default_Deleter<element_type>>
        explicit Shared_Ptr(element_type* iObject, Deleter deleter = Deleter());
        /// Конструктор копирования
        Shared_Ptr(const Shared_Ptr& other);
        /// Конструктор перемещеиня
        Shared_Ptr(Shared_Ptr&& other) noexcept;
        /// Деструктор
        ~Shared_Ptr();
        /// Оператор копирования
        Shared_Ptr& operator=(const Shared_Ptr& other);
        /// Оператор перемещения
        Shared_Ptr& operator=(Shared_Ptr&& other) noexcept;
        Shared_Ptr& operator=(decltype(nullptr));
        //auto operator<=>(const Shared_Ptr&) const = default; // сравнение по-умолчанию
        bool operator==(const Shared_Ptr& other); // Особый случай
        element_type* Get() noexcept;
        const element_type* Get() const noexcept;
        element_type& operator*() noexcept;
        element_type* operator->() noexcept;
        // operator const void*() const noexcept; // object == nullptr
        [[nodiscard]] element_type& operator[](uint64_t i); // C++17: можно обращаться к элементам массива
        void Swap(Shared_Ptr& other);
        [[nodiscard("Unique")]] bool Unique() const noexcept;
        [[nodiscard("Use_Count")]] uint64_t Use_Count() const noexcept;
        
        // Reset - менее безопасный чем Make_Shared
        void Reset(TClass* ptr = nullptr);
        
    private:
        element_type* _ptr;
        ControlBlock* _controlBlock;
        std::function<void(element_type*)> _deleter;
    };

    template <>
    class Shared_Ptr<void> // Запрет на создание void
    {
        Shared_Ptr() = delete; // Удаление конструктора
    };

    /// Конструктор по-умолчанию
    template <class TClass>
    Shared_Ptr<TClass>::Shared_Ptr() noexcept : _ptr(nullptr), _controlBlock(nullptr)
    {
        std::cout << "Constructor nullptr" << std::endl;
    }

    template <class TClass>
    Shared_Ptr<TClass>::Shared_Ptr(decltype(nullptr)) noexcept
    {
        Shared_Ptr();
    }

    template <class TClass>
    template <typename Deleter>
    Shared_Ptr<TClass>::Shared_Ptr(element_type* ptr, Deleter deleter) :
    _ptr(ptr),
    _controlBlock(ptr ? new ControlBlock() : nullptr),
    _deleter(deleter)
    {
        std::cout << "Constructor new" << std::endl;
    }

    /// Конструктор копирования
    template <class TClass>
    Shared_Ptr<TClass>::Shared_Ptr(const Shared_Ptr& other)
    {
        std::cout << "Сopy constructor: ";
        *this = other;
    }

    /// Конструктор перемещения
    template <class TClass>
    Shared_Ptr<TClass>::Shared_Ptr(Shared_Ptr&& other) noexcept
    {
        std::cout << "Move constructor: ";
        *this = std::move(other);
    }

    /// Деструктор
    template <class TClass>
    Shared_Ptr<TClass>::~Shared_Ptr()
    {
        if (_ptr && _controlBlock)
        {
            std::cout << "Destructor: ";
            if (--_controlBlock->_shared_count == 0)
            {
                _deleter(_ptr);
                _ptr = nullptr;
                std::cout << "delete object" << std::endl;

                if (_controlBlock->_weak_count == 0)
                {
                    delete _controlBlock;
                    _controlBlock = nullptr;
                    std::cout << "delete control block" << std::endl;
                }
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
                std::cout << "decrease _shared_count - " << _controlBlock->_shared_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
    }

    /// Оператор копирования
    template <class TClass>
    Shared_Ptr<TClass>& Shared_Ptr<TClass>::operator=(const Shared_Ptr& other)
    {   
        if (this == &other) // object = object
            return *this;
        
        if (_controlBlock) // Shared_Ptr<T> object1(object2);
        {
            if (--_controlBlock->_shared_count == 0)  // object1 = object2
            {
                _deleter(_ptr);
                _ptr = nullptr;
                std::cout << "delete object" << std::endl;
                  
                if (_controlBlock->_weak_count == 0)
                {
                    delete _controlBlock;
                    _controlBlock = nullptr;
                    std::cout << "delete control block" << std::endl;
                }
            }
            else
                std::cout << "decrease _shared_count - " << _controlBlock->_shared_count << std::endl;
        }
        
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        _deleter = other._deleter;
        ++_controlBlock->_shared_count;
        std::cout << "operator=: increase _shared_count - " << _controlBlock->_shared_count << std::endl;
        return *this;
    }

    /// Оператор перемещения
    template <class TClass>
    Shared_Ptr<TClass>& Shared_Ptr<TClass>::operator=(Shared_Ptr&& other) noexcept
    {
        if (this == &other) // object = std::move(object)
            return *this;
        
        if (_controlBlock) // Shared_Ptr<T> object1(std::move(object2));
        {
            if (--_controlBlock->_shared_count == 0) // object1 = std::move(object2)
            {
                _deleter(_ptr);
                _ptr = nullptr;
                std::cout << "delete object" << std::endl;
                  
                if (_controlBlock->_weak_count == 0)
                {
                    delete _controlBlock;
                    _controlBlock = nullptr;
                    std::cout << "delete control block" << std::endl;
                }
            }
            else
                std::cout << "decrease _shared_count - " << _controlBlock->_shared_count << std::endl;
        }
        
        _ptr = std::exchange(other._ptr, nullptr); // std::move не работает с сырыми указателями
        _controlBlock = std::exchange(other._controlBlock, nullptr); // std::move не работает с сырыми указателями
        _deleter = std::move(other._deleter); // std::move не работает с сырыми указателями
        std::cout << "operator=" << std::endl;
        return *this;
    }

    template <class TClass>
    Shared_Ptr<TClass>& Shared_Ptr<TClass>::operator=(decltype(nullptr))
    {
        if (_controlBlock) // Shared_Ptr<T> object1 = nullptr
        {
            if (--_controlBlock->_shared_count == 0) // object1 = nullptr
            {
                _deleter(_ptr);
                _ptr = nullptr;
                std::cout << "delete object" << std::endl;
                  
                if (_controlBlock->_weak_count == 0)
                {
                    delete _controlBlock;
                    _controlBlock = nullptr;
                    std::cout << "delete control block" << std::endl;
                }
            }
            else
                std::cout << "decrease _shared_count - " << _controlBlock->_shared_count << std::endl;
        }
        return *this;
    }

    template <class TClass>
    bool Shared_Ptr<TClass>::operator==(const Shared_Ptr& other)
    {
        return _ptr == other._ptr && _controlBlock == other._controlBlock; // сравниваем адреса указателей
    }

    template <class TClass>
    std::remove_extent_t<TClass>* Shared_Ptr<TClass>::Get() noexcept
    {
        return _ptr ? _ptr : nullptr;
    }

    template <class TClass>
    const std::remove_extent_t<TClass>* Shared_Ptr<TClass>::Get() const noexcept
    {
        return _ptr ? _ptr : nullptr;
    }

    template <class TClass>
    std::remove_extent_t<TClass>& Shared_Ptr<TClass>::operator*() noexcept
    {
        return *Get();
    }

    template <class TClass>
    std::remove_extent_t<TClass>* Shared_Ptr<TClass>::operator->() noexcept
    {
        return Get();
    }

    /*
     template <class TClass>
     Shared_Ptr<TClass>::operator const void*() const noexcept
     {
         return Get(); // object == nullptr
     }
     */

    /// C++17: можно обращаться к элементам массива
    template <class TClass>
    std::remove_extent_t<TClass>& Shared_Ptr<TClass>::operator[](uint64_t i)
    {
        return *Get();
    }

    template <class TClass>
    void Shared_Ptr<TClass>::Swap(Shared_Ptr& other)
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_controlBlock, other._controlBlock);
        std::swap(_ptr, other._ptr);
    }

    template <class TClass>
    bool Shared_Ptr<TClass>::Unique() const noexcept
    {
        return _controlBlock ? _controlBlock->_shared_count.load() == 1 : false;
    }

    template <class TClass>
    uint64_t Shared_Ptr<TClass>::Use_Count() const noexcept
    {
        return _controlBlock ? _controlBlock->_shared_count.load() : 0;
    }

    // Reset - менее безопасный чем Make_Shared
    template <class TClass>
    void Shared_Ptr<TClass>::Reset(TClass* ptr)
    {
        if (_controlBlock)
        {
            if (--_controlBlock->_shared_count == 0)
            {
                _deleter(_ptr);
                _ptr = nullptr;
                std::cout << "delete object" << std::endl;
                  
                if (_controlBlock->_weak_count == 0)
                {
                    delete _controlBlock;
                    _controlBlock = nullptr;
                    std::cout << "delete control block" << std::endl;
                }
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
                std::cout << "decrease _shared_count - " << _controlBlock->_shared_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
        
        if (ptr)
        {
            _ptr = ptr;
            _controlBlock = new ControlBlock();
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
    Shared_Ptr<TClass> Make_Shared(TArgs&& ...iArgs)
    {
        // std::allocate_shared<TClass>(allocator<TClass>(), std::forward<_Args>(iArgs)...);
        using element_type = std::remove_extent_t<TClass>; // C++17: Для поддержки использования типа Make_Shared<int[]>(10);
        return Shared_Ptr<TClass>(new element_type(std::forward<TArgs>(iArgs)...));
    }
}

#endif /* Shared_Ptr_h */
