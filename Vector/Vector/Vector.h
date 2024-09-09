#ifndef Vector_h
#define Vector_h

#include "Allocator.h"
#include "ReverseVector.h"

#include <algorithm>
#include <iostream>

/*
 Лекция: https://www.youtube.com/watch?v=kUqXNSgdd5A&ysclid=lu8lgbqu7g137468251
 Сайты: https://github.com/jakjan95/vector/blob/main/include/vector.hpp
        https://github.com/chetvertakov/Vector/blob/master/vector.h
        https://github.com/skayfish/Vector/blob/master/vector.h
        https://github.com/MaximShichanin/Vector/blob/master/vector.h
 */

// Класс, который помогает вызывать деструкторы и освобождение памяти при выходе из зоны видимости или при расткрутке стека в try/catch
template <class T, typename Allocator = Allocator<T>>
class Vector_Base
{
    // Копирование не требуется, потому что там своя семантика
    Vector_Base(const Vector_Base& other) = delete;
    Vector_Base& operator=(const Vector_Base& other) = delete;
    
public:
    Vector_Base() noexcept = default;
    
    explicit Vector_Base(size_t capacity) :
    _allocator(Allocator()),
    _data(_allocator.Allocate(capacity)),
    _capacity(capacity)
    {
        
    }
    
    Vector_Base(Vector_Base&& other) noexcept :
    _allocator(std::exchange(other._allocator, {})),
    _data(std::exchange(other._data, nullptr)), // move не работает с указателями
    _size(std::exchange(other._size, 0u)),
    _capacity(std::exchange(other._capacity, 0u))
    {
        
    }
    
    ~Vector_Base()
    {
        while (_size-- > 0)
            _allocator.Destructor(_data + _size);
        _allocator.Deallocate(_data);
        _data = nullptr;
    }
    
    Vector_Base& operator=(Vector_Base&& other) noexcept
    {
        if (this == &other) // object = object
            return *this;
        
        _allocator = std::exchange(other._allocator, {});
        _data = std::exchange(other._data, nullptr); // move не работает с указателями
        _size = std::exchange(other._size, 0u);
        _capacity = std::exchange(other._capacity, 0u);
        
        return *this;
    }
    
    T* operator+(size_t index)
    {
        if (index > _capacity)
            throw std::out_of_range("Index is out of range!");
        
        return _data + index;
    }

    const T* operator+(size_t index) const noexcept
    {
        return const_cast<Vector_Base&>(*this) + index;
    }
    
    T& operator[](size_t index) noexcept 
    {
        if (index > _capacity)
            throw std::out_of_range("Index is out of range!");

        return _data[index];
    }

    const T& operator[](size_t index) const noexcept 
    {
        return const_cast<Vector_Base&>(*this)[index];
    }
    
    void Swap(Vector_Base &other) noexcept
    {
        std::swap(_allocator, other._allocator);
        std::swap(_data, other._data);
        std::swap(_size, other._size);
        std::swap(_capacity, other._capacity);
    }
    
protected:
    Allocator _allocator;
    T* _data = nullptr;
    size_t _size = 0u;
    size_t _capacity = 0u;
};


template <class T>
class Vector : private Vector_Base<T>
{
    // Выносим на 2 стадию инстанцирования, чтобы можно было использовать protected члены без Vector_Base<T>
    using Vector_Base<T>::_allocator;
    using Vector_Base<T>::_data;
    using Vector_Base<T>::_size;
    using Vector_Base<T>::_capacity;
    
    using size_type = size_t;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = ReverseIterator<T>;
    using const_reverse_iterator = ReverseIterator<T>;
    
    // В качестве примера можно создать аллокатор на другой тип
    using custom_allocator = Allocator<T>::template Rebind<size_type>::other;
    
public:
    Vector() = default;
    ~Vector() = default;
    explicit Vector(size_type count, const value_type& value = value_type()) noexcept; // Вызовется на +1 больше конструктор по умолчанию!!! Обычно это выносится в отдельный конструктор
    Vector(const std::initializer_list<T>& vector) noexcept;
    Vector(const Vector& other);
    Vector(Vector&& other) noexcept;
    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;
    bool operator==(const Vector& other) const;
    bool operator!=(const Vector& other) const;
    reference operator[](size_type index);
    const_reference operator[](size_type index) const;
    
    void Push_Back(const T& value);
    void Push_Back(T&& value);
    template <typename ...Args>
    decltype(auto) Emplace_Back(Args&& ...args); // decltype(auto) - не отбрасывает ссылки и возвращает lvalue, иначе rvalue
    void Pop_Back();
    reference At(size_type index);
    const_reference At(size_type index) const;
    reference Front();
    const_reference Front() const;
    reference Back();
    const_reference Back() const;
    void Swap(Vector& other) noexcept;
    bool Empty() const noexcept;
    size_type Size() const noexcept;
    void Resize(size_type size);
    size_type Capacity() const noexcept;
    void Reserve(size_type capacity);
    void Shrink_To_Fit() noexcept;
    iterator Data() noexcept;
    const_iterator Data() const noexcept;
    void Fill(const value_type& value) noexcept;
    void Clear() noexcept;
    
    template <typename ...Args>
    iterator Emplace(const_iterator it, Args&& ...args);
    iterator Insert(const_iterator it, const T& value);
    iterator Erase(const_iterator it);
    iterator Erase(const_iterator begin, const_iterator end);
    
    iterator Begin() noexcept;
    iterator End() noexcept;
    const_iterator Begin() const noexcept;
    const_iterator End() const noexcept;
    const_iterator CBegin() const noexcept;
    const_iterator CEnd() const noexcept;
    
    reverse_iterator RBegin() noexcept;
    reverse_iterator REnd() noexcept;
    const_reverse_iterator CRBegin() const noexcept;
    const_reverse_iterator CREnd() const noexcept;
    
private:
    void Destroy();
};


template <class T>
void Vector<T>::Destroy()
{
    // Перед удаление нужно вызвать деструкторы для элементов < size, для остальных элементов выделялась только сырая память
    if (_data)
    {
        for (size_type i = 0; i < _size; ++i)
            _allocator.Destructor(_data + i);
        operator delete(_data);
        _data = nullptr;
        _size = 0u;
        _capacity = 0u;
    }
}

template <class T>
Vector<T>::Vector(size_type count, const T& value) noexcept:
Vector_Base<T>(count)
{
    for (size_type i = 0; i < count; ++i)
    {
        _allocator.Constructor(_data + _size, value);
        ++_size;
    }
}

template <class T>
Vector<T>::Vector(const std::initializer_list<T>& vector) noexcept : 
Vector_Base<T>(vector.size())
{
    for (const auto &elem : vector)
    {
        _allocator.Constructor(_data + _size, elem);
        ++_size;
    }
}

template <class T>
Vector<T>::Vector(const Vector& other) :
Vector_Base<T>(other._capacity)
{
    while (_size < other._size)
    {
        _allocator.Constructor(_data + _size, other._data[_size]);
        ++_size;
    }
}

// Можно использовать default в C++20
template <class T>
Vector<T>::Vector(Vector&& other) noexcept:
Vector_Base<T>(std::move(other))
{
    _size = std::exchange(other._size, 0u);
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector& other)
{
    if (this == &other) // object = object
        return *this;

    Vector(other).Swap(*this);
    
    return *this;
}

// Можно использовать default в C++20
template <class T>
Vector<T>& Vector<T>::operator=(Vector&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    Vector_Base<T>::operator=(std::move(other));
    _size = std::exchange(other._size, 0u);
    
    return *this;
}

template <class T>
bool Vector<T>::operator==(const Vector& other) const
{
    if (this == &other) // object = object
        return true;
    
    if (Size() != other.Size())
        return false;
    
    for (auto it = CBegin(), it_other = other.Begin(); it != End(); ++it, ++it_other)
    {
        if (*it != *it_other)
            return false;
    }
    
    return true;
}

template <class T>
bool Vector<T>::operator!=(const Vector& other) const
{
    return !(*this == other);
}

template <class T>
Vector<T>::reference Vector<T>::operator[](size_type index)
{
    return _data[index];
}

template <class T>
Vector<T>::const_reference Vector<T>::operator[](size_type index) const
{
    return _data[index];
}

template <class T>
void Vector<T>::Push_Back(const T& value)
{
    T tmp(value); // В Push_Back(T&& value) будет проверка на noexcept в перемещении, иначе будет копирование
    Emplace_Back(std::move(tmp));
}

template <class T>
void Vector<T>::Push_Back(T&& value)
{
    Emplace_Back(std::move(value));
}

template <class T>
template <typename ...Args>
decltype(auto) Vector<T>::Emplace_Back(Args&& ...args) // decltype(auto) - не отбрасывает ссылки и возвращает lvalue, иначе rvalue
{
    if (_size == _capacity)
    {
        Reserve(_capacity * 2 + 1); // +1 потому что может быть 0
        
        // _data[_size] = T(std::forward<Args>(args)...); // вызовет деструктор для созданного объекта _data[_size], но если будет сырая память то будет undefined behaiver
        _allocator.Constructor(_data + _size, std::forward<Args>(args)...); // Добавляем новый элемент в конец
    }
    else
    {
        // _data[_size] = T(std::forward<Args>(args)...); // вызовет деструктор для созданного объекта _data[_size], но если будет сырая память то будет undefined behaiver
        _allocator.Constructor(_data + _size, std::forward<Args>(args)...); // Добавляем новый элемент в конец
    }
    
    ++_size;
    return Back();
}

template <class T>
void Vector<T>::Pop_Back()
{
    if (Empty())
        throw std::range_error("Vector is empty");
    
    Destructor(End());
    --_size;
}

template <class T>
Vector<T>::reference Vector<T>::At(size_type index)
{
    if (index >= _size)
        throw std::out_of_range("Index is out of range!");
    
    return _data[index];
}

template <class T>
Vector<T>::const_reference Vector<T>::At(size_type index) const
{
    if (index >= _size)
        throw std::out_of_range("Index is out of range!");
    
    return _data[index];
}

template <class T>
Vector<T>::reference Vector<T>::Front()
{
    return _data[0];
}

template <class T>
Vector<T>::const_reference Vector<T>::Front() const
{
    return _data[0];
}

template <class T>
Vector<T>::reference Vector<T>::Back()
{
    return _data[_size - 1];
}

template <class T>
Vector<T>::const_reference Vector<T>::Back() const
{
    return _data[_size - 1];
}

template <class T>
void Vector<T>::Swap(Vector& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    Vector_Base<T>::Swap(other);
}

template <class T>
bool Vector<T>::Empty() const noexcept
{
    return Size() == 0;
}

template <class T>
Vector<T>::size_type Vector<T>::Size() const noexcept
{
    return _size;
}

template <class T>
void Vector<T>::Resize(size_type size)
{
    if (size < _size)
    {
        /*
         Делает тоже самое, что и ниже
         std::destroy_n(_data + size, _size - size);
         */
        {
            while (_size-- < size)
                Destructor(_data + _size);
        }
    } 
    else if (_size < size)
    {
        Reserve(size);
        /*
         Делает тоже самое, что и ниже
         std::uninitialized_value_construct_n(_data + _size, size - _size);
         */
        {
            while (_size < size)
            {
                Constructor(_data + _size); // Вызов конструктора по умолчанию
                ++_size;
            }
        }
    }
}

template <class T>
Vector<T>::size_type Vector<T>::Capacity() const noexcept
{
    return _capacity;
}

template <class T>
void Vector<T>::Reserve(size_type capacity)
{
    if (capacity <= Capacity())
        return;
    
    Vector_Base<T> tmp(capacity);
    auto& tmp_size = reinterpret_cast<Vector<T>&>(tmp)._size;
    
    /*
     Делает тоже самое, что и ниже
     if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
     {
         std::uninitialized_move_n(_data, _size, tmp._data);
     }
     else
     {
         std::uninitialized_copy_n(_data, _size, tmp._data);
     }
     std::destroy_n(_data, _size);
    */
    {
        /*
         В конструкторе копирования должна присутствовать строгая гарантия исключений: если при копировании при реолокации памяти вылетает исключение - идет откат к состоянию, которое было перед выполнением копирования.
         В конструкторе перемещения/операторе перемещения должна присутствовать гарантия отсутсвия исключений и помечены как noexcept: при перемещении при реолокации памяти вылетает исключение - старый буфер остался в несогласованном состоянии и в новом до конца не допереместили, поэтому при вылете исключения должно быть abort/terminate.
         */
        for (tmp_size = 0; tmp_size < _size; ++tmp_size)
        {
            // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
            if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
            {
                // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                _allocator.Constructor(tmp + tmp_size, std::move(_data[tmp_size]));
            }
            else
            {
                // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                _allocator.Constructor(tmp + tmp_size, _data[tmp_size]);
            }
        }
        
        Destroy(); // Очищаем старые данные
    }
    
    tmp.Swap(*this);
}

template <class T>
void Vector<T>::Shrink_To_Fit() noexcept
{
    if (_capacity > _size)
    {
        Vector_Base<T> tmp(_capacity = _size);
        auto& tmp_size = reinterpret_cast<Vector<T>&>(tmp)._size;
        
        /*
         Делает тоже самое, что и ниже
         if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
         {
            std::uninitialized_move_n(_data, _size, tmp._data);
         }
         else
         {
            std::uninitialized_copy_n(_data, _size, tmp._data);
         }
         std::destroy_n(_data, _size);
         */
        {
            /*
             В конструкторе копирования должна присутствовать строгая гарантия исключений: если при копировании при реолокации памяти вылетает исключение - идет откат к состоянию, которое было перед выполнением копирования.
             В конструкторе перемещения/операторе перемещения должна присутствовать гарантия отсутсвия исключений и помечены как noexcept: при перемещении при реолокации памяти вылетает исключение - старый буфер остался в несогласованном состоянии и в новом до конца не допереместили, поэтому при вылете исключения должно быть abort/terminate.
             */
            for (tmp_size = 0; tmp_size < _size; ++tmp_size)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    _allocator.Constructor(tmp + tmp_size, std::move(_data[tmp_size]));
                }
                else
                {
                    // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    _allocator.Constructor(tmp + tmp_size, _data[tmp_size]);
                }
            }
            
            Destroy(); // Очищаем старые данные
        }
        
        tmp.Swap(*this);
    }
}

template <class T>
Vector<T>::iterator Vector<T>::Data() noexcept
{
    return _data;
}

template <class T>
Vector<T>::const_iterator Vector<T>::Data() const noexcept
{
    return _data;
}

template <class T>
void Vector<T>::Fill(const value_type& value) noexcept
{
    for (size_type i = 0; i < _size; ++i)
        _data[i] = value;
}

template <class T>
void Vector<T>::Clear() noexcept
{
    Destroy();
}

template <class T>
template <typename ...Args>
Vector<T>::iterator Vector<T>::Emplace(Vector<T>::const_iterator it, Args&& ...args)
{
    size_type index = static_cast<size_type>(it - Begin());
    
    if (it == End())
    {
        return &Emplace_Back(std::forward<Args>(args)...);
    }
    if (_size == Capacity())
    {
        Vector_Base<T> tmp(_size * 2 + 1); // +1 потому что может быть 0
        auto& tmp_size = reinterpret_cast<Vector<T>&>(tmp)._size;
        new (tmp + index) T(std::forward<Args>(args)...);
        // Делает тоже самое, что и ниже
        /*
         if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
         {
             std::uninitialized_move_n(_data, index, tmp);
         }
         else
         {
             try
             {
                 std::uninitialized_copy_n(_data, index, tmp);
             }
             catch (...)
             {
                 std::destroy_n(tmp + index, 1u);
             }
         }

         if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
         {
             std::uninitialized_move_n(_data + index, _size - index, tmp + index + 1);
         }
         else
         {
             try
             {
                 std::uninitialized_copy_n(_data + index, _size - index, tmp + index + 1);
             }
             catch (...)
             {
                 std::destroy_n(tmp, index + 1);
             }
         }
         std::destroy_n(_data, _size);
         */
        {
            /*
             В конструкторе копирования должна присутствовать строгая гарантия исключений: если при копировании при реолокации памяти вылетает исключение - идет откат к состоянию, которое было перед выполнением копирования.
             В конструкторе перемещения/операторе перемещения должна присутствовать гарантия отсутсвия исключений и помечены как noexcept: при перемещении при реолокации памяти вылетает исключение - старый буфер остался в несогласованном состоянии и в новом до конца не допереместили, поэтому при вылете исключения должно быть abort/terminate.
             */
            
            // Левая часть от нового элемента
            for (tmp_size = 0; tmp_size < index; ++tmp_size)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    _allocator.Constructor(tmp + tmp_size, std::move(_data[tmp_size]));
                }
                else
                {
                    // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    _allocator.Constructor(tmp + tmp_size, _data[tmp_size]);
                }
            }
            
            // Правая часть от нового элемента
            for (tmp_size = index; tmp_size < _size; ++tmp_size)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i + 1] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i + 1], но если будет сырая память то будет undefined behaiver
                    _allocator.Constructor(tmp + tmp_size + 1, std::move(_data[tmp_size]));
                }
                else
                {
                    // tmp[i + 1] = _data[i]; // вызовет деструктор для созданного объекта tmp[i + 1], но если будет сырая память то будет undefined behaiver
                    _allocator.Constructor(tmp + tmp_size + 1, _data[tmp_size]);
                }
            }
            
            Destroy();
        }
        
        tmp.Swap(*this);
    }
    else
    {
        // память после последнего элемента - не инициализирована, поэтому инициализируем её размещающим new, остальные элементы переносим на один вправо
        _allocator.Constructor(End(), std::move(_data[_size - 1]));
        std::move_backward(Begin() + index, End() - 1, End());
        _data[index] = T(std::forward<Args>(args)...);
    }
    
    ++_size;
    return Begin() + index;
}

template <class T>
Vector<T>::iterator Vector<T>::Insert(Vector<T>::const_iterator it, const T& value)
{
    return Emplace(it, value);
}

template <class T>
Vector<T>::iterator Vector<T>::Erase(Vector<T>::const_iterator it)
{
    if (Empty())
        throw std::range_error("Vector is empty");
    
    size_t index = static_cast<size_t>(it - Begin());
    std::move(Begin() + index + 1, End(), Begin() + index);
    _allocator.Destructor(End());
    --_size;
    if (_size == 0)
    {
        operator delete(_data);
        _data = nullptr;
        _capacity = 0;
    }
    return Begin() + index;
}

template <class T>
Vector<T>::iterator Vector<T>::Erase(Vector<T>::const_iterator begin, Vector<T>::const_iterator end)
{
    size_t index = static_cast<size_t>(end - begin);
    auto it = Vector<T>::iterator(begin);
    while (index-- > 0)
        it = Erase(it);
    return it;
}

template <class T>
Vector<T>::iterator Vector<T>::Begin() noexcept
{
    return iterator(_data);
};

template <class T>
Vector<T>::iterator Vector<T>::End() noexcept
{
    return iterator(_data + _size);
};

template <class T>
Vector<T>::const_iterator Vector<T>::Begin() const noexcept
{
    return const_iterator(_data);
};

template <class T>
Vector<T>::const_iterator Vector<T>::End() const noexcept
{
    return const_iterator(_data + _size);
};

template <class T>
Vector<T>::const_iterator Vector<T>::CBegin() const noexcept
{
    return Begin();
};

template <class T>
Vector<T>::const_iterator Vector<T>::CEnd() const noexcept
{
    return End();
};

template <class T>
Vector<T>::reverse_iterator Vector<T>::RBegin() noexcept
{
    return reverse_iterator(&_data[0] + _size - 1);
}

template <class T>
Vector<T>::reverse_iterator Vector<T>::REnd() noexcept
{
    return reverse_iterator(&_data[0] - 1);
}

template <class T>
Vector<T>::const_reverse_iterator Vector<T>::CRBegin() const noexcept
{
    return const_reverse_iterator(&_data[0] + _size - 1);
}

template <class T>
Vector<T>::const_reverse_iterator Vector<T>::CREnd() const noexcept
{
    return const_reverse_iterator(&_data[0] - 1);
}

namespace massive
{
    template <class T, size_t N>
    T* Begin(T (&vector)[N])
    {
        return vector;
    }

    template <class T, size_t N>
    T* End(T (&vector)[N])
    {
        return vector + N;
    }

    template<class T, size_t N>
    ReverseIterator<T> RBegin(T (&vector)[N])
    {
        return ReverseIterator<T>(&vector[0] + N - 1);
    }

    template<class T, size_t N>
    ReverseIterator<T> REnd(T (&vector)[N])
    {
        return ReverseIterator<T>(&vector[0] - 1);
    }
}

#endif /* Vector_h */
