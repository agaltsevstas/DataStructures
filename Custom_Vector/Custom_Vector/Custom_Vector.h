#ifndef Custom_Vector_h
#define Custom_Vector_h

#include "ReverseIterator.h"

#include <algorithm>
#include <iostream>


/*
 Лекция: https://www.youtube.com/watch?v=kUqXNSgdd5A&ysclid=lu8lgbqu7g137468251
 Сайты: https://github.com/jakjan95/vector/blob/main/include/vector.hpp
        https://github.com/chetvertakov/Vector/blob/master/vector.h
        https://github.com/skayfish/Vector/blob/master/vector.h
        https://github.com/MaximShichanin/Vector/blob/master/vector.h
 */

template <class T>
class Custom_Vector
{
    using size_type = size_t;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = ReverseIterator<T>;
    using const_reverse_iterator = ReverseIterator<T>;
    
public:
    Custom_Vector() = default;
    ~Custom_Vector() = default;
    explicit Custom_Vector(size_type size, const value_type& value = value_type()); // Вызовется на +1 больше конструктор по умолчанию!!! Обычно это выносится в отдельный конструктор
    Custom_Vector(const std::initializer_list<T>& vector);
    Custom_Vector(const Custom_Vector& other);
    Custom_Vector(Custom_Vector&& other) noexcept;
    Custom_Vector& operator=(const Custom_Vector& other);
    Custom_Vector& operator=(Custom_Vector&& other) noexcept;
    bool operator==(const Custom_Vector& other) const;
    bool operator!=(const Custom_Vector& other) const;
    reference operator[](size_type index);
    const_reference operator[](size_type index) const;
    
    void Push_Back(const T& value);
    void Push_Back(T&& value);
    template <typename ...Args>
    decltype(auto) Emplace_Back(Args&& ...args);
    void Pop_Back();
    reference At(size_type index);
    const_reference At(size_type index) const;
    reference Front();
    const_reference Front() const;
    reference Back();
    const_reference Back() const;
    void Swap(Custom_Vector& other) noexcept;
    bool Empty() const noexcept;
    size_type Size() const noexcept;
    void Resize(size_type size);
    size_type Capacity() const noexcept;
    void Reserve(size_type capacity);
    void Shrink_To_Fit();
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
    void Destroy()
    {
        // Перед удаление нужно вызвать деструкторы для элементов < size, для остальных элементов выделялась только сырая память
        for (size_type i = 0; i < _size; ++i)
            _data[i].~T();
        operator delete(_data);
        _data = nullptr;
    }
    
private:
    T* _data;
    size_t _size = 0u;
    size_t _capacity = 0u;
};


template <class T>
Custom_Vector<T>::Custom_Vector(size_type count, const T& value)
{
    _capacity = count;
    if (_capacity > 0)
    {
        // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
        
        // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
        _data = static_cast<T*>(operator new(_capacity * sizeof(T)));
        try
        {
            for (_size = 0; _size < count; ++_size)
            {
                // _data[_size] = other._data[_size]; // вызовет деструктор для созданного объекта _data[_size], но если будет сырая память то будет undefined behaiver
                new (_data + _size) T(value); // placement new: создаем объект в выделенной памяти
            }
        }
        catch (...)
        {
            while (_size-- < 0)
                _data[_size].~T();
            operator delete(_data);
            throw; // Пробрасываем исключения
        }
    }
}

template <class T>
Custom_Vector<T>::Custom_Vector(const std::initializer_list<T>& vector)
{
    _capacity = vector.size();
    if (_capacity > 0)
    {
        // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
        
        // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
        _data = static_cast<T*>(operator new(_capacity * sizeof(T)));
        try
        {
            for (_size = 0; const auto &elem : vector)
            {
                // _data[_size] = other._data[_size]; // вызовет деструктор для созданного объекта _data[_size], но если будет сырая память то будет undefined behaiver
                new (_data + _size) T(elem); // placement new: создаем объект в выделенной памяти
                ++_size;
            }
        }
        catch (...)
        {
            while (_size-- < 0)
                _data[_size].~T();
            operator delete(_data);
            throw; // Пробрасываем исключения
        }
    }
}

template <class T>
Custom_Vector<T>::Custom_Vector(const Custom_Vector& other)
{
    _capacity = other._capacity;
    if (_capacity > 0)
    {
        // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
        _data = static_cast<T*>(operator new(_capacity * sizeof(T)));
        try
        {
            // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
            
            for (_size = 0; _size < other._size; ++_size)
            {
                // _data[_size] = other._data[_size]; // вызовет деструктор для созданного объекта _data[_size], но если будет сырая память то будет undefined behaiver
                new (_data + _size) T(other._data[_size]); // placement new: создаем объект в выделенной памяти
            }
        }
        catch (...)
        {
            while (_size-- < 0)
                _data[_size].~T();
            operator delete(_data);
            throw; // Пробрасываем исключения
        }
    }
}

template <class T>
Custom_Vector<T>::Custom_Vector(Custom_Vector&& other) noexcept :
_data(std::exchange(other._data, nullptr)), // move не работает с указателями
_size(std::exchange(other._size, 0u)),
_capacity(std::exchange(other._capacity, 0u))
{
    
}

template <class T>
Custom_Vector<T>& Custom_Vector<T>::operator=(const Custom_Vector& other)
{
    if (this == &other) // object = object
        return *this;

    Custom_Vector tmp(other);
    Swap(tmp);
    
    return *this;
}

template <class T>
Custom_Vector<T>& Custom_Vector<T>::operator=(Custom_Vector&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    _data = std::exchange(other._data, nullptr); // move не работает с указателями
    _size = std::exchange(other._size, 0u);
    _capacity = std::exchange(other._capacity, 0u);
    
    return *this;
}

template <class T>
bool Custom_Vector<T>::operator==(const Custom_Vector& other) const
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
bool Custom_Vector<T>::operator!=(const Custom_Vector& other) const
{
    return !(*this == other);
}

template <class T>
Custom_Vector<T>::reference Custom_Vector<T>::operator[](size_type index)
{
    return _data[index];
}

template <class T>
Custom_Vector<T>::const_reference Custom_Vector<T>::operator[](size_type index) const
{
    return _data[index];
}

template <class T>
template <typename ...Args>
decltype(auto) Custom_Vector<T>::Emplace_Back(Args&& ...args)
{
    if (_size == _capacity)
    {
        _capacity = std::max(_capacity * 2, (size_t)1); // При 0 будет 1 элемент
        
        // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
        
        // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
        T* tmp = static_cast<T*>(operator new(_capacity * sizeof(T)));
        size_type i = 0;
        try
        {
            /*
             Строгая Гарантия безопасности исключений: перемещать можно, если конструктор перемещения и оператора перемещения помечены как noexcept.
             Пример, если мы сделали реолакацию и при move вылетело исключение и старый буфер остался в несогласованном состоянии и в новом мы еще ничего не доперемещали, поэтому при вылете исключения должно быть terminate.
             */
            for (; i < _size; ++i)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    new (tmp + i) T(std::move(_data[i])); // placement new: создаем объект в выделенной памяти
                }
                else
                {
                    // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    new (tmp + i) T(_data[i]); // placement new: создаем объект в выделенной памяти
                }
            }
            
            Destroy();
            _data = tmp;
            new (End()) T(std::forward<Args>(args)...); // placement new
        }
        catch (...)
        {
            while (i-- < 0)
                tmp[i].~T();
            operator delete(tmp);
            throw; // Пробрасываем исключения
        }
    }
    else
    {
        new (End()) T(std::forward<Args>(args)...); // placement new
    }
    
    ++_size;
    return _data[_size - 1];
}

template <class T>
void Custom_Vector<T>::Push_Back(const T& value)
{
    T tmp(value); // В Push_Back(T&& value) будет проверка на noexcept в перемещении, иначе будет копирование
    Emplace_Back(std::move(tmp));
}

template <class T>
void Custom_Vector<T>::Push_Back(T&& value)
{
    Emplace_Back(std::move(value));
}

template <class T>
void Custom_Vector<T>::Pop_Back()
{
    if (Empty())
        throw std::range_error("Vector is empty");
    
    _data[_size--]->~T();
}

template <class T>
Custom_Vector<T>::reference Custom_Vector<T>::At(size_type index)
{
    if (index >= _size)
        throw std::out_of_range("Index is out of range!");
    
    return _data[index];
}

template <class T>
Custom_Vector<T>::const_reference Custom_Vector<T>::At(size_type index) const
{
    if (index >= _size)
        throw std::out_of_range("Index is out of range!");
    
    return _data[index];
}

template <class T>
Custom_Vector<T>::reference Custom_Vector<T>::Front()
{
    return _data[0];
}

template <class T>
Custom_Vector<T>::const_reference Custom_Vector<T>::Front() const
{
    return _data[0];
}

template <class T>
Custom_Vector<T>::reference Custom_Vector<T>::Back()
{
    return _data[_size - 1];
}

template <class T>
Custom_Vector<T>::const_reference Custom_Vector<T>::Back() const
{
    return _data[_size - 1];
}

template <class T>
void Custom_Vector<T>::Swap(Custom_Vector& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    std::swap(_data, other._data);
    std::swap(_size, other._size);
    std::swap(_capacity, other._capacity);
}

template <class T>
bool Custom_Vector<T>::Empty() const noexcept
{
    return Size() == 0;
}

template <class T>
Custom_Vector<T>::size_type Custom_Vector<T>::Size() const noexcept
{
    return _size;
}

template <class T>
void Custom_Vector<T>::Resize(size_type size)
{
    if (size < _size)
    {
        while (_size < size)
        {
            _data[_size - 1].~T();
            --_size;
        }
    }
    else if (_size < size)
    {
        Reserve(size);
        while (_size < size)
        {
            new (_data + _size) T();
            ++_size;
        }
    }
}

template <class T>
Custom_Vector<T>::size_type Custom_Vector<T>::Capacity() const noexcept
{
    return _capacity;
}

template <class T>
void Custom_Vector<T>::Reserve(size_type capacity)
{
    if (capacity <= Capacity())
        return;
    
    _capacity = capacity;
    // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
    
    // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
    T* tmp = static_cast<T*>(operator new(_capacity * sizeof(T)));
    size_type i = 0;
    try
    {
        /*
         Строгая Гарантия безопасности исключений: перемещать можно, если конструктор перемещения и оператора перемещения помечены как noexcept.
         Пример, если мы сделали реолакацию и при move вылетело исключение и старый буфер остался в несогласованном состоянии и в новом мы еще ничего не доперемещали, поэтому при вылете исключения должно быть terminate.
         */
        for (size_type i = 0; i < _size; ++i)
        {
            // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
            if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
            {
                // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                new (tmp + i) T(std::move(_data[i])); // placement new: создаем объект в выделенной памяти
            }
            else
            {
                // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                new (tmp + i) T(_data[i]); // placement new: создаем объект в выделенной памяти
            }
        }
        
        Destroy();
        _data = tmp;
    }
    catch (...)
    {
        while (i-- < 0)
            tmp[i].~T();
        operator delete(tmp);
        throw; // Пробрасываем исключения
    }
}

template <class T>
void Custom_Vector<T>::Shrink_To_Fit()
{
    if (_capacity > _size)
    {
        _capacity = _size;
        // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
        
        // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
        T* tmp = static_cast<T*>(operator new(_capacity * sizeof(T)));
        size_type i = 0;
        try
        {
            /*
             Строгая Гарантия безопасности исключений: перемещать можно, если конструктор перемещения и оператора перемещения помечены как noexcept.
             Пример, если мы сделали реолакацию и при move вылетело исключение и старый буфер остался в несогласованном состоянии и в новом мы еще ничего не доперемещали, поэтому при вылете исключения должно быть terminate.
             */
            
            for (size_type i = 0; i < _size; ++i)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    new (tmp + i) T(std::move(_data[i])); // placement new: создаем объект в выделенной памяти
                }
                else
                {
                    // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    new (tmp + i) T(_data[i]); // placement new: создаем объект в выделенной памяти
                }
            }
            
            Destroy();
            _data = tmp;
        }
        catch (...)
        {
            while (i-- < 0)
                tmp[i].~T();
            operator delete(tmp);
            throw; // Пробрасываем исключения
        }
    }
}

template <class T>
Custom_Vector<T>::iterator Custom_Vector<T>::Data() noexcept
{
    return _data;
}

template <class T>
Custom_Vector<T>::const_iterator Custom_Vector<T>::Data() const noexcept
{
    return _data;
}

template <class T>
void Custom_Vector<T>::Fill(const value_type& value) noexcept
{
    for (size_type i = 0; i < _size; ++i)
        _data[i] = value;
}

template <class T>
void Custom_Vector<T>::Clear() noexcept
{
    Destroy();
    _size = 0u;
    _capacity = 0u;
}

template <class T>
template <typename ...Args>
Custom_Vector<T>::iterator Custom_Vector<T>::Emplace(Custom_Vector<T>::const_iterator it, Args&& ...args)
{
    size_type index = static_cast<size_type>(it - Begin());
    
    if (it == End())
    {
        return &Emplace_Back(std::forward<Args>(args)...);
    }
    if (_size == Capacity())
    {
        _capacity = std::max(_capacity * 2, (size_t)1); // При 0 будет 1 элемент
        // T* tmp = new T(_capacity * sizeof(T)); Вызвался бы дефолтный конструктор, а хотелось бы хранить пока только выделенную память, чтобы при освобождении памяти не нужно было вызывать деструктор созданного объекта
        
        // Не вызовет дефолтный конструктор, а выделит сырую память, при bad_alloc утечки памяти не будет, поэтому просто выходим из метода
        T* tmp = static_cast<T*>(operator new(_capacity * sizeof(T)));
        new (tmp + index) T(std::forward<Args>(args)...); // placement new: создаем объект в выделенной памяти
        
        /*
         Строгая Гарантия безопасности исключений: перемещать можно, если конструктор перемещения и оператора перемещения помечены как noexcept.
         Пример, если мы сделали реолакацию и при move вылетело исключение и старый буфер остался в несогласованном состоянии и в новом мы еще ничего не доперемещали, поэтому при вылете исключения должно быть terminate.
         */
        size_type i = 0;
        try
        {
            // Левая часть от нового элемента
            for (; i < index; ++i)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    new (tmp + index) T(std::move(_data[i])); // placement new: создаем объект в выделенной памяти
                }
                else
                {
                    // tmp[i] = _data[i]; // вызовет деструктор для созданного объекта tmp[i], но если будет сырая память то будет undefined behaiver
                    new (tmp + index) T(_data[i]); // placement new: создаем объект в выделенной памяти
                }
            }
        }
        catch (...)
        {
            while (i-- < 0)
                tmp[i].~T();
            operator delete(tmp);
            throw; // Пробрасываем исключения
        }
            
        try
        {
            // Правая часть от нового элемента
            for (i = index; i < _size; ++i)
            {
                // Проверка, что тип имеет конструктор перемещения и оператор перемещения с noexcept
                if constexpr (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
                {
                    // tmp[i + 1] = std::move(_data[i]); // вызовет деструктор для созданного объекта tmp[i + 1], но если будет сырая память то будет undefined behaiver
                    new (tmp + index + 1) T(std::move(_data[i])); // placement new: создаем объект в выделенной памяти
                }
                else
                {
                    // tmp[i + 1] = _data[i]; // вызовет деструктор для созданного объекта tmp[i + 1], но если будет сырая память то будет undefined behaiver
                    new (tmp + index + 1) T(_data[i]); // placement new: создаем объект в выделенной памяти
                }
            }
        }
        catch (...)
        {
            while (i-- >= index)
                tmp[i].~T();
            operator delete(tmp);
            throw; // Пробрасываем исключения
        }
        
        Destroy();
        _data = tmp;
    }
    else
    {
        // память после последнего элемента - не инициализирована, поэтому инициализируем её размещающим new, остальные элементы переносим на один вправо
        new (End()) T(std::move(_data[_size - 1]));
        std::move_backward(Begin() + index, End() - 1, End());
        _data[index] = T(std::forward<Args>(args)...);
    }
    
    ++_size;
    return Begin() + index;
}

template <class T>
Custom_Vector<T>::iterator Custom_Vector<T>::Insert(Custom_Vector<T>::const_iterator it, const T& value)
{
    return Emplace(it, value);
}

template <class T>
Custom_Vector<T>::iterator Custom_Vector<T>::Erase(Custom_Vector<T>::const_iterator it)
{
    if (Empty())
        throw std::range_error("Vector is empty");
    
    size_t index = static_cast<size_t>(it - Begin());
    std::move(Begin() + index + 1, End(), Begin() + index);
    End()->~T();
    --_size;
    return Begin() + index;
}

template <class T>
Custom_Vector<T>::iterator Custom_Vector<T>::Erase(Custom_Vector<T>::const_iterator begin, Custom_Vector<T>::const_iterator end)
{
    size_t index = static_cast<size_t>(end - begin);
    auto it = Custom_Vector<T>::iterator(begin);
    while (index-- > 0)
        it = Erase(it);
    return it;
}

template <class T>
Custom_Vector<T>::iterator Custom_Vector<T>::Begin() noexcept
{
    return iterator(Data());
};

template <class T>
Custom_Vector<T>::iterator Custom_Vector<T>::End() noexcept
{
    return iterator(Data() + _size);
};

template <class T>
Custom_Vector<T>::const_iterator Custom_Vector<T>::Begin() const noexcept
{
    return iterator(Data());
};

template <class T>
Custom_Vector<T>::const_iterator Custom_Vector<T>::End() const noexcept
{
    return iterator(Data() + _size);
};

template <class T>
Custom_Vector<T>::const_iterator Custom_Vector<T>::CBegin() const noexcept
{
    return Begin();
};

template <class T>
Custom_Vector<T>::const_iterator Custom_Vector<T>::CEnd() const noexcept
{
    return End();
};

template <class T>
Custom_Vector<T>::reverse_iterator Custom_Vector<T>::RBegin() noexcept
{
    return reverse_iterator(&_data[0] + _size - 1);
}

template <class T>
Custom_Vector<T>::reverse_iterator Custom_Vector<T>::REnd() noexcept
{
    return reverse_iterator(&_data[0] - 1);
}

template <class T>
Custom_Vector<T>::const_reverse_iterator Custom_Vector<T>::CRBegin() const noexcept
{
    return const_reverse_iterator(&_data[0] + _size - 1);
}

template <class T>
Custom_Vector<T>::const_reverse_iterator Custom_Vector<T>::CREnd() const noexcept
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

#endif /* Custom_Vector_h */
