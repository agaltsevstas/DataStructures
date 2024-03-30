#ifndef Allocator_h
#define Allocator_h

#include <iostream>

// Свой аллокатор вместо вызова напрямую new
template <typename T>
struct Allocator
{
    using value_type = T;
    using size_type = std::size_t;

    // Выделение сырой памяти без вызовов конструкторов
    T* Allocate(size_type capacity);
    // Освобождение памяти без вызовов деструкторов
    void Deallocate(T* ptr);
    // Вызов конструктора
    template <typename ...Args>
    void Constructor(T* ptr, Args&& ...args);
    // Вызов деструктора
    void Destructor(T* ptr);
    // Позволяет создавать аллокатор для другого типа
    template <typename U>
    struct Rebind
    {
        using other = Allocator<U>;
    };
};

// Выделение сырой памяти без вызовов конструкторов
template <typename T>
T* Allocator<T>::Allocate(size_type capacity)
{
    return capacity > 0 ? static_cast<T*>(operator new(capacity * sizeof(T))) : nullptr;
}

// Освобождение памяти без вызовов деструкторов
template <typename T>
void Allocator<T>::Deallocate(T* ptr)
{
    // delete[] ptr // Вызовет деструкторы по всем элементам и потом удалит
    if (ptr)
    {
        operator delete(ptr);
        ptr = nullptr;
    }
}

// Вызов конструктора
template <class T>
template <typename ...Args>
void Allocator<T>::Constructor(T* ptr, Args&& ...args)
{
    // ptr[i] = T(std::forward<Args>(args)... // вызовет деструктор для созданного объекта ptr[i], но если будет сырая память то будет undefined behaiver
    new (ptr) T(std::forward<Args>(args)...); // placement new: создаем объект в выделенной памяти
}

// Вызов деструктора
template <class T>
void Allocator<T>::Destructor(T* ptr)
{
    ptr->~T();
}

#endif /* Allocator_h */
