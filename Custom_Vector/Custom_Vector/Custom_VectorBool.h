#ifndef Custom_VectorBool_h
#define Custom_VectorBool_h

#include "Custom_Vector.h"


/*
 Лекция: https://www.youtube.com/watch?v=kUqXNSgdd5A&ysclid=lu8lgbqu7g137468251
 Сайты: https://github.com/jakjan95/vector/blob/main/include/vector.hpp
        https://volstr.ru/36/?ysclid=lu8iuv9nps4898588
 */

// Выделение сырой памяти
template <typename T>
static T* Allocate(size_t capacity)
{
    return capacity > 0 ? static_cast<T*>(operator new(capacity * sizeof(T))) : nullptr;
}

// Освобождение памяти
template <typename T>
static void Deallocate(T* ptr)
{
    // delete[] ptr // Вызовет деструкторы по всем элементам и потом удалит
    if (ptr)
    {
        operator delete(ptr);
        ptr = nullptr;
    }
}

/*
 Контрпример, где ломает общие правила в Vector
 Хотелось бы при вызове operator[] менялся один бит вместо 1 байта(bool == 8 битам)
 */

/// Явная специализация, для реализации bool тратиться вместо  байта - 1 бит
template<>
class Custom_Vector<bool>
{
    using value_type = char;
    using size_type = std::size_t;
public:
    Custom_Vector(size_type count, const bool& value)
    {
        _size = count;
        _capacity = GetCapacityValueForAllocatedSpace(count);
        _data = Allocate<value_type>(GetNumberOfBlocksTypeToAllocateSpace(count));
        std::uninitialized_fill_n(_data, GetNumberOfBlocksTypeToAllocateSpace(count), value_type {});
        for (size_type i = 0; i < count; ++i)
        {
            SetValueAtPosition(i, value);
        }
    }
    
    ~Custom_Vector()
    {
        Deallocate(_data);
    }
private:
    struct BitReference // Хранит указатель ячейку в VectorBool
    {
        constexpr BitReference(value_type& iValue, value_type iMask) :
        value(iValue),
        mask(iMask)
        { }
        
        constexpr BitReference& operator=(bool b)
        {
            if (b)
                // поразрядное ИЛИ
                value |= mask; // записать 1 в бит
            else
                // поразрядное И
                value &= ~mask; // записать 0 в бит
            
            return *this;
        }

        constexpr BitReference& operator=(const BitReference& b)
        {
            return *this = bool(b);
        }

        constexpr operator bool() const
        {
            // поразрядное И
            return !!(value & mask);
        }

        constexpr void flip()
        {
            // поразрядное исключающее ИЛИ
            value ^= mask; // Инвертирование
        }

        value_type& value;
        value_type mask;
    };
    
public:
    BitReference operator[](size_t index) // В данном случае возвращается rvalue
    {
        const auto [blockWithBit, mask] = GetBlockWithBitAndMask(index);
        return BitReference(_data[blockWithBit], mask);
    }
    
    size_type Size() const noexcept
    {
        return _size;
    }
    
    size_type Capacity() const noexcept
    {
        return _capacity;
    }
    
private:
    constexpr inline auto GetBlockCapacity() const noexcept { return sizeof(value_type) * 8; }
    inline size_type GetNumberOfBlocksTypeToAllocateSpace(size_type count) const
    {
        return static_cast<size_type>(std::ceil(static_cast<double>(count) / static_cast<double>(GetBlockCapacity())));
    }
    
    inline size_type GetCapacityValueForAllocatedSpace(size_type count) const
    {
        return GetNumberOfBlocksTypeToAllocateSpace(count) * GetBlockCapacity();
    }
    
    constexpr std::pair<size_type, size_type> GetBlockWithBitAndMask(size_type index) const
    {
        const auto blockWithBit = index / GetBlockCapacity();
        const auto bitPositionInBlock = index % GetBlockCapacity();
        const auto mask = 1ULL << bitPositionInBlock;
        return std::make_pair(blockWithBit, mask);
    }
    
    constexpr void SetValueAtPosition(size_type index, bool value)
    {
        const auto [blockWithBit, mask] = GetBlockWithBitAndMask(index);
        _data[blockWithBit] ^= (-value ^ _data[blockWithBit]) & mask;
    }
    
private:
    value_type* _data;
    size_type _size;
    size_type _capacity;
};

#endif /* Custom_VectorBool_h */
