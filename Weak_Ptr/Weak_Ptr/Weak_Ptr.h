#ifndef Weak__ptrh
#define Weak__ptrh

#include "Shared_Ptr.h"

/*
 Лекция: https://www.youtube.com/watch?v=9ZSBOfTd-sc&ab_channel=%D0%9C%D0%B5%D1%89%D0%B5%D1%80%D0%B8%D0%BD%D0%98%D0%BB%D1%8C%D1%8F
 Сайты: https://github.com/google/shipshape/blob/master/third_party/proto/src/google/protobuf/stubs/shared_ptr.h
 */

/*
 Weak_Ptr (слабый указатель - не владаеет ресурсом): создает с помощью lock - Shared_Ptr, но не выделяет память для Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter) - это делает сам Shared_Ptr. При вызове конструктора, конструктора копирования или оператора копирования ++weak_count. При вызове конструктора перемещения, оператора перемещения или деструктора --Weak_Ptr. При shared_count == 0 + Weak_Ptr == 0 удаляет  Control Block, но НЕ УДАЛЯЕТ сам объект, это может сделать ТОЛЬКО - Shared_Ptr.
 Weak_Ptr нужен для решения проблемы с «висячим указателем» или «циклической зависимостью», где два объекта взаимноссылаются на друг друга и при выходе видимости стека их деструкторы не будут вызваны -> утечка памяти. Идет в связке с Shared_Ptr, но не увеличивает счетчик ссылок.
 из std::Weak_Ptr легко создать умный указатель std::Shared_Ptr, из сырого указателя - это низкоуровневое управление динамической памятью (гиблое дело).
 При создании нескольких std::Shared_Ptr из «сырых указателей» создается свой контрольный блок, они оказываются несвязанные друг с другом и это приводит к двойному удалению объекта.
 При создании нескольких std::Shared_Ptr из std::Weak_Ptr, они будут иметь один контрольный блок.
 При условии, что в std::Weak_Ptr есть объект, с помощью метода lock() создается std::Shared_Ptr.
 expired() - проверяет объект на nullptr.
 */

namespace STD
{
    template <typename TClass>
    class Weak_Ptr
    {
        Weak_Ptr& operator=(decltype(nullptr)) = delete;
    public:
        /// Конструктор по-умолчанию
        Weak_Ptr() noexcept;
        /// Конструктор для Shared_Ptr
        Weak_Ptr(const Shared_Ptr<TClass>& shared_ptr) noexcept;
        /// Конструктор копирования
        Weak_Ptr(const Weak_Ptr& other) noexcept;
        /// Конструктор перемещеиня
        Weak_Ptr(Weak_Ptr&& other) noexcept;
        /// Деструктор
        ~Weak_Ptr() noexcept;
        /// Оператор копирования для Shared_Ptr
        Weak_Ptr& operator=(const Shared_Ptr<TClass>& shared_ptr);
        /// Оператор копирования
        Weak_Ptr& operator=(const Weak_Ptr& other);
        /// Оператор перемещения
        Weak_Ptr& operator=(Weak_Ptr&& other);
        //auto operator<=>(const Weak_Ptr&) const = default; // сравнение по-умолчанию
        bool operator==(const Weak_Ptr& other); // Особый случай
        void Swap(Weak_Ptr& other);
        [[nodiscard("Use_Count")]] uint64_t Use_Count() const noexcept;
        bool Expired() const noexcept;
        Shared_Ptr<TClass> Lock() const;
        // Reset - менее безопасный чем Make_Shared
        void Reset();

    private:
        TClass* _ptr;
        ControlBlock* _controlBlock;
    };

    template <>
    class Weak_Ptr<void> // Запрет на создание void
    {
        Weak_Ptr() = delete; // Удаление конструктора
    };

    /// Конструктор по-умолчанию
    template <class TClass>
    Weak_Ptr<TClass>::Weak_Ptr() noexcept : _ptr(nullptr), _controlBlock(nullptr)
    {
        std::cout << "Constructor nullptr" << std::endl;
    }

    /// Конструктор для Shared_Ptr
    template <class TClass>
    Weak_Ptr<TClass>::Weak_Ptr(const Shared_Ptr<TClass>& shared_ptr) noexcept :
    _ptr(shared_ptr.ptr),
    _controlBlock(shared_ptr._controlBlock)
    {
        if (shared_ptr._controlBlock)
            ++shared_ptr._controlBlock->_weak_count;
        std::cout << "Constructor Shared_Ptr" << std::endl;
    }

    /// Конструктор копирования
    template <class TClass>
    Weak_Ptr<TClass>::Weak_Ptr(const Weak_Ptr& other) noexcept
    {
        std::cout << "Сopy constructor: ";
        *this = other;
    }

    /// Конструктор перемещения
    template <class TClass>
    Weak_Ptr<TClass>::Weak_Ptr(Weak_Ptr&& other) noexcept
    {
        std::cout << "Move constructor: ";
        *this = std::move(other);
    }

    /// Деструктор
    template <class TClass>
    Weak_Ptr<TClass>::~Weak_Ptr() noexcept
    {
        if (_controlBlock)
        {
            if (--_controlBlock->_weak_count == 0 && _controlBlock->_shared_count == 0)
            {
                delete _controlBlock;
                _controlBlock = nullptr;
                std::cout << "delete control block" << std::endl;
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
                std::cout << "decrease _weak_ptr - " << _controlBlock->_weak_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
    }

    /// Оператор копирования для Shared_Ptr
    template <class TClass>
    Weak_Ptr<TClass>& Weak_Ptr<TClass>::operator=(const Shared_Ptr<TClass>& shared_ptr)
    {
        if (_ptr == shared_ptr._ptr)
        {
            ++_controlBlock->_weak_count;
            return *this;
        }
        
        if (_controlBlock) // Weak_Ptr<T> object(shared_ptr);
        {
            if (--_controlBlock->_weak_count == 0 && _controlBlock->_shared_count == 0)
            {
                delete _controlBlock;
                _controlBlock = nullptr;
                std::cout << "delete control block" << std::endl;
            }
            else
            {
                std::cout << "decrease _weak_ptr - " << _controlBlock->_weak_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
        
        _ptr = shared_ptr._ptr;
        _controlBlock = shared_ptr._controlBlock;
        ++_controlBlock->_weak_count;
        std::cout << "operator=: increase _weak_ptr - " << _controlBlock->_weak_count << std::endl;
        return *this;
    }

    /// Оператор копирования
    template <class TClass>
    Weak_Ptr<TClass>& Weak_Ptr<TClass>::operator=(const Weak_Ptr& other)
    {
        if (this == &other) // object = object
            return *this;
        
        if (_controlBlock) // Weak_Ptr<T> object1(object2);
        {
            if (--_controlBlock->_weak_count == 0 && _controlBlock->_shared_count == 0)
            {
                delete _controlBlock;
                _controlBlock = nullptr;
                std::cout << "delete control block" << std::endl;
            }
            else
            {
                std::cout << "decrease _weak_ptr - " << _controlBlock->_weak_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
        
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        ++_controlBlock->_weak_count;
        std::cout << "operator=: increase _weak_ptr - " << _controlBlock->_weak_count << std::endl;
        return *this;
    }

    /// Оператор перемещения
    template <class TClass>
    Weak_Ptr<TClass>& Weak_Ptr<TClass>::operator=(Weak_Ptr&& other)
    {
        if (this == &other) // object = std::move(object)
            return *this;
        
        if (_controlBlock) // Weak_Ptr<T> object1(std::move(object2));
        {
            if (--_controlBlock->_weak_count == 0 && _controlBlock->_shared_count == 0) // object1 = std::move(object2)
            {
                delete _controlBlock;
                _controlBlock = nullptr;
                std::cout << "delete control block" << std::endl;
            }
            else
            {
                std::cout << "decrease _weak_ptr - " << _controlBlock->_weak_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
        
        _ptr = std::exchange(other._ptr, nullptr); // std::move не работает с сырыми указателями
        _controlBlock = std::exchange(other._controlBlock, nullptr); // std::move не работает с сырыми указателями
        std::cout << "operator=" << std::endl;
        return *this;
    }

    template <class TClass>
    bool Weak_Ptr<TClass>::operator==(const Weak_Ptr& other)
    {
        return _ptr == other._ptr && _controlBlock == other._controlBlock; // сравниваем адреса указателей
    }

    template <class TClass>
    void Weak_Ptr<TClass>::Swap(Weak_Ptr& other)
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_ptr, other._ptr);
        std::swap(_controlBlock, other._controlBlock);
    }

    template <class TClass>
    uint64_t Weak_Ptr<TClass>::Use_Count() const noexcept
    {
        return _controlBlock ? _controlBlock->_shared_count.load() : 0;
    }

    template <class TClass>
    bool Weak_Ptr<TClass>::Expired() const noexcept
    {
        return Use_Count() == 0;
    }

    template <class TClass>
    Shared_Ptr<TClass> Weak_Ptr<TClass>::Lock() const
    {
        if (!_controlBlock)
            return nullptr;
        
        Shared_Ptr<TClass> result(_ptr);
        result._controlBlock = _controlBlock;
        ++result._controlBlock->_shared_count;

        return result;
    }

    template <class TClass>
    void Weak_Ptr<TClass>::Reset()
    {
        if (_controlBlock)
        {
            if (--_controlBlock->_weak_count == 0 && _controlBlock->_shared_count == 0)
            {
                delete _controlBlock;
                _controlBlock = nullptr;
                std::cout << "delete control block" << std::endl;
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
                std::cout << "decrease _weak_ptr - " << _controlBlock->_weak_count << std::endl; // иногда при удалении копии nullptr слетает
            }
        }
    }
}

#endif /* Weak__ptrh */
