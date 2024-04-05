#ifndef Unordered_Map_h
#define Unordered_Map_h

#include <iostream>
#include <list>
#include <vector>


/*
 Сайты: https://github.com/VladimirBalun/Algorithms/blob/master/DataStructures/HashTableWithSeparateChaining.cpp
        https://github.com/carlaoutput/hashtable/blob/master/hashtable.hpp
 */

template <typename T>
struct Equal_To
{
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs == rhs;
    }
};

template <class Key,
          class Value,
          class Hash = std::hash<Key>,
          class Equal = Equal_To<Key>>
class Unordered_Map
{
public:
    using size_type = size_t;
    using value_type = std::pair<const Key, Value>; // ключ не может меняться, поэтому const
    using reference = value_type&;
    using const_reference = const value_type&;
    
private:
    struct ListNode
    {
    public:
        ListNode() = default;
        ~ListNode() = default;
        ListNode(const value_type& iData, size_type iBucket):
        data(iData),
        bucket(iBucket)
        {
            
        }
        
        ListNode(const ListNode& other)
        {
            operator=(other);
        }
        
        ListNode(ListNode&& other) noexcept
        {
            operator=(std::move(other));
        }
        
        ListNode& operator=(const ListNode& other)
        {
            if (this == &other) // object = object
                return *this;
            
            *const_cast<Key*>(&data.first) = data.first;
            *const_cast<Value*>(&data.second) = data.second;
            bucket = other.bucket;
            
            return *this;
        }
        
        ListNode& operator=(ListNode&& other) noexcept
        {
            if (this == &other) // object = object
                return *this;
            
            *const_cast<Key*>(&data.first) = std::move(data.first);
            *const_cast<Value*>(&data.second) = std::move(data.second);
            bucket = std::exchange(other.bucket, 0u);
            
            return *this;
        }
        
        bool operator==(const ListNode& other) const
        {
            return data == other.data && bucket == other.bucket;
        }
        
        value_type data;
        size_type bucket; // принадлежность к bucket
    };
    
    using list_type = std::list<ListNode>;
    
public:
    using const_value_type = const std::pair<const Key, Value>;
    class Iterator;
    using Const_Iterator = const Iterator;
    
private:
    using buckets_t = std::vector<Iterator>;
    
public:
    Unordered_Map() = default;
    ~Unordered_Map() = default;
    
    Unordered_Map(const std::initializer_list<value_type>& map) noexcept;
    Unordered_Map(const Unordered_Map& other);
    Unordered_Map(Unordered_Map&& other) noexcept;
    Unordered_Map& operator=(const Unordered_Map& other);
    Unordered_Map& operator=(Unordered_Map&& other) noexcept;
    bool operator==(const Unordered_Map& other) const noexcept;
    bool operator!=(const Unordered_Map& other) const noexcept;
    // Менее эффективно - создается значение по умолчанию, а потом происходит присвоение
    Value& operator[](const Key& key);
    // Создает ключ со значением по умолчанию
    const Value& operator[](const Key& key) const;
    Value& At(const Key& key);
    const Value& At(const Key& key) const;
    
    // (Amortized time: O(1)), но при коллизиях требуется пройтись по всему односвязному списку (list) - это занимает время (Time: O(n))
    template <typename ...Args>
    std::pair<Iterator, bool> Emplace(Args&& ...args);
    std::pair<Iterator, bool> Insert(const_value_type& element);
    // TODO: кладет рядом с итератором, если bucket не сильно отличаются, время стремится -> Time: O(1)
    std::pair<Iterator, bool> Insert(Const_Iterator it, const_value_type& element);
    // (Time: O(1))
    Iterator Find(const Key& key) const noexcept;
    // (Time: O(1))
    size_type Count(const Key& key) const noexcept;
    // (Time: O(1))
    bool Contains(const Key& key) const noexcept;
    // (Time: O(1))
    Iterator Erase(const Key& key);
    // (Time: O(1))
    Iterator Erase(Const_Iterator it);
    // (Time: O(n))
    Iterator Erase(Const_Iterator begin, Const_Iterator end);
    
    // При вставки нового элемента при условии load_factor > max_load_factor происходит перераспределение: хэш-значения у элементов остаются такими же, но порядок хранения в buckets меняется из-за увеличения остататка от деления (% buckets) -> (% 2*buckets). (Time: O(n))
    void Rehash(size_type count);
    void Swap(Unordered_Map& other) noexcept;
    
    // list_size / buckets_size (vector_size). При load_factor происходит rehash
    float Load_Factor() const noexcept;
    /*
     Порог, который можно установить вручную (по-умолчанию 1.0). Это максимально допустимое соотношение между числом элементов и количеством корзин (count/buckets). Если при вставке очередного элемента среднее число элементов в корзинах превышает этот порог, число корзин (buckets) увеличивается и происходит рехеширование. Если заранее известно количество ключей, то можно сделать reserve и избежать лишних рехеширований при вставках.
     */
    float Max_Load_Factor() const noexcept;
    void Max_Load_Factor(float max_factor) noexcept;
    // hash % buckets - вычисление принадлежности хэш-значения к бакету
    size_type Bucket(const Key& key) const;
    // vector_index_size - кол-во элементов в списке в рамках одного bucket
    size_type Bucket_Size(size_type index) const;
    // vector_size - кол-во buckets
    size_type Buckets_Count() const noexcept;
    bool Empty() const noexcept;
    size_type Size() const noexcept;
    void Clear() noexcept;
    
    Iterator Begin() noexcept;
    Iterator End() noexcept;
    Const_Iterator Begin() const noexcept;
    Const_Iterator End() const noexcept;
    Const_Iterator CBegin() const noexcept;
    Const_Iterator CEnd() const noexcept;
    
private:
    buckets_t _buckets;
    list_type _list;
    float _max_factor = 1.0; // по-умолчанию
    size_type _size = 0;
};

// Декоратор, который разрешает обращаться к data, но запрещает обращение к bucket
template <class Key,
          class Value,
          class Hash,
          class Equal>
class Unordered_Map<Key, Value, Hash, Equal>::Iterator
{
    friend class Unordered_Map;
public:
    Iterator() = default;
    Iterator(list_type::iterator it) :
    _iterator(it)
    {
        _isValid = true;
    }
    
    inline value_type& operator*()
    {
        return _iterator->data;
    }
    
    inline const value_type& operator*() const
    {
        return _iterator->data;
    }

    inline value_type* operator->()
    {
        return &_iterator->data;
    }
    
    inline const value_type* operator->() const
    {
        return &_iterator->data;
    }
    
    inline bool operator==(const Iterator& other) const
    {
        if (_isValid && other._isValid)
            return _iterator == other._iterator;
        return _isValid == other._isValid;
    }
    
    inline bool operator!=(const Iterator& other) const
    {
        return !(*this == other);
    }

    inline bool operator==(list_type::iterator iterator) const
    {
        return _isValid && _iterator == iterator;
    }

    inline bool operator!=(list_type::iterator iterator) const
    {
        return !(*this == iterator);
    }
    
    inline Iterator& operator++()
    {
        ++_iterator;
        return *this;
    }

    inline Iterator operator++(int)
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }

    inline Iterator& operator--()
    {
        --_iterator;
        return *this;
    }

    inline Iterator operator--(int)
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }

private:
    list_type::iterator get()
    {
        return _iterator;
    }

    list_type::iterator get() const
    {
        return _iterator;
    }

    size_type& bucket()
    {
        return _iterator->bucket;
    }

    const size_type& bucket() const
    {
        return _iterator->bucket;
    }
    
private:
    bool _isValid = false;
    list_type::iterator _iterator;
};


template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Unordered_Map(const std::initializer_list<value_type>& map) noexcept
{
    Rehash(map.size());
    for (const auto &elem : map)
        Insert(elem);
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Unordered_Map(const Unordered_Map& other)
{
    _buckets = other._buckets;
    _list = other._list;
    _max_factor = other._max_factor;
    _size = other._size;
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Unordered_Map(Unordered_Map&& other) noexcept
{
    _buckets = std::move(other._buckets);
    _list = std::move(other._list);
    _max_factor = std::exchange(other._max_factor, 0.0);
    _size = std::exchange(other._size, 0u);
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>& Unordered_Map<Key, Value, Hash, Equal>::operator=(const Unordered_Map& other)
{
    if (this == &other) // object = object
        return *this;
    
    _buckets = other._buckets;
    _list = other._list;
    _max_factor = other._max_factor;
    _size = other._size;
    
    return *this;
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>& Unordered_Map<Key, Value, Hash, Equal>::operator=(Unordered_Map&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    _buckets = std::move(other._buckets);
    _list = std::move(other._list);
    _max_factor = std::exchange(other._max_factor, 0.0f);
    _size = std::exchange(other._size, 0);
    
    return *this;
}

template <class Key, class Value, class Hash, class Equal>
bool Unordered_Map<Key, Value, Hash, Equal>::operator==(const Unordered_Map& other) const noexcept
{
    if (this == &other) // object = object
        return true;
    
    return _buckets == other._buckets &&
           _list == other._list &&
           _max_factor == other._max_factor &&
           _size == other._size;
}

template <class Key, class Value, class Hash, class Equal>
bool Unordered_Map<Key, Value, Hash, Equal>::operator!=(const Unordered_Map& other) const noexcept
{
    return !(*this == other);
}

// Менее эффективно - создается значение по умолчанию, а потом происходит присвоение
template <class Key, class Value, class Hash, class Equal>
Value& Unordered_Map<Key, Value, Hash, Equal>::operator[](const Key& key)
{
    auto [it, flag] = Emplace(std::move(std::make_pair(key, Value())));
    return it->second;
}

// Создает ключ со значением по умолчанию
template <class Key, class Value, class Hash, class Equal>
const Value& Unordered_Map<Key, Value, Hash, Equal>::operator[](const Key& key) const
{
    auto [it, flag] = Emplace(std::move(std::make_pair(key, Value())));
    return it->second;
}

template <class Key, class Value, class Hash, class Equal>
Value& Unordered_Map<Key, Value, Hash, Equal>::At(const Key& key)
{
    auto it = Find(key);
    if (it == Iterator())
        throw std::runtime_error("Key is not exist!");
    
    return it->second;
}

template <class Key, class Value, class Hash, class Equal>
const Value& Unordered_Map<Key, Value, Hash, Equal>::At(const Key& key) const
{
    auto it = Find(key);
    if (it == Iterator())
        throw std::runtime_error("Key is not exist!");
      
    return it->second;
}

// (Amortized time: O(1)), но при коллизиях требуется пройтись по всему односвязному списку (list) - это занимает время (Time: O(n))
template <class Key, class Value, class Hash, class Equal>
template <typename ...Args>
std::pair<typename Unordered_Map<Key, Value, Hash, Equal>::Iterator, bool> Unordered_Map<Key, Value, Hash, Equal>::Emplace(Args&& ...args)
{
    auto element = value_type(std::forward<Args>(args)...); // В случае exception элемент не добавится
    if (Load_Factor() >= Max_Load_Factor())
        Rehash(Buckets_Count() * 2 + 1);
    
    auto& [key, value] = element;
    size_type bucket = Bucket(key);
    auto it = _buckets[bucket];
    if (it != Iterator())
    {
        while (it.get() != _list.end() && it.bucket() == bucket)
        {
            if (Equal()(it->first, element.first))
                return {it, false};
            ++it;
        }
        
        it = _list.emplace(it.get(), std::move(element), bucket);
    }
    else
    {
        _list.emplace_front(std::move(element), bucket);
        _buckets[bucket] = _list.begin();
        it = _list.begin();
    }

    ++_size;
    return {it, true};
}

template <class Key, class Value, class Hash, class Equal>
std::pair<typename Unordered_Map<Key, Value, Hash, Equal>::Iterator, bool> Unordered_Map<Key, Value, Hash, Equal>::Insert(const_value_type& element)
{
    return Emplace(std::move(element));
}

// TODO: кладет рядом с итератором, если bucket не сильно отличаются, время стремится -> Time: O(1)
template <class Key, class Value, class Hash, class Equal>
std::pair<typename Unordered_Map<Key, Value, Hash, Equal>::Iterator, bool> Unordered_Map<Key, Value, Hash, Equal>::Insert(Const_Iterator it, const_value_type& element)
{
    
}

// (Time: O(1))
template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Iterator Unordered_Map<Key, Value, Hash, Equal>::Find(const Key& key) const noexcept
{
    size_type bucket = Bucket(key);
    if (bucket < _buckets.size())
    {
        if (auto it = _buckets[bucket]; it != Iterator())
        {
            for (; it.get() != _list.end() && it.bucket() == bucket; ++it)
            {
                if (Equal()(it->first, key))
                {
                    return Iterator(it);
                }
            }
        }
    }
    
    return Iterator();
}

// (Time: O(1))
template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::size_type Unordered_Map<Key, Value, Hash, Equal>::Count(const Key& key) const noexcept
{
    return Find(key) != Iterator() ? 1u : 0u;
}

// (Time: O(1))
template <class Key, class Value, class Hash, class Equal>
bool Unordered_Map<Key, Value, Hash, Equal>::Contains(const Key& key) const noexcept
{
    return Find(key) != Iterator();
}

// (Time: O(1))
template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Iterator Unordered_Map<Key, Value, Hash, Equal>::Erase(const Key& key)
{
    Iterator result = Find(key);
    if (result == End())
        return result;
    
    return Erase(result);
}

// (Time: O(1))
template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Iterator Unordered_Map<Key, Value, Hash, Equal>::Erase(Const_Iterator it)
{
    if (it != Iterator())
    {
        size_type bucket = it.bucket();
        if (it == _buckets[bucket])
        {
            auto result = _list.erase(it.get());
            if (result != _list.end() && result->bucket == bucket)
                _buckets[bucket] = result;
            return result;
        }
        else
        {
            return _list.erase(it.get());
        }
    }
    
    return it;
}

// (Time: O(n))
template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Iterator Unordered_Map<Key, Value, Hash, Equal>::Erase(Const_Iterator begin, Const_Iterator end)
{
    auto it = begin;
    while (it != end)
        it = Erase(it);
    return it;
}

// При вставки нового элемента при условии load_factor > max_load_factor происходит перераспределение: хэш-значения у элементов остаются такими же, но порядок хранения в buckets меняется из-за увеличения остататка от деления (% buckets) -> (% 2*buckets). (Time: O(n))
template <class Key, class Value, class Hash, class Equal>
void Unordered_Map<Key, Value, Hash, Equal>::Rehash(size_type count)
{
    buckets_t buckets(count);
    list_type list;
    
    for (auto iter = _list.begin(); iter != _list.end(); ++iter)
    {
        auto [key, value] = iter->data;
        size_type bucket = Hash()(key) % buckets.size();
        auto it = buckets[bucket];
        if (it != Iterator())
        {
            while (it != list.end() && it.bucket() == bucket)
                ++it;
            
            list.emplace(it.get(), std::move(iter->data), bucket);
        }
        else
        {
            list.emplace_front(std::move(iter->data), bucket);
            buckets[bucket] = list.begin();
        }
    }
    
    _list = std::move(list);
    _buckets = std::move(buckets);
}

template <class Key, class Value, class Hash, class Equal>
void Unordered_Map<Key, Value, Hash, Equal>::Swap(Unordered_Map& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    std::swap(_buckets, other._buckets);
    std::swap(_list, other._list);
    std::swap(_max_factor, other._max_factor);
    std::swap(_size, other._size);
}

// list_size / buckets_size (vector_size). При load_factor происходит rehash
template <class Key, class Value, class Hash, class Equal>
float Unordered_Map<Key, Value, Hash, Equal>::Load_Factor() const noexcept
{
    if (_buckets.empty())
        return 1.0f;

    return _size / static_cast<float>(_buckets.size());
}

/*
 Порог, который можно установить вручную (по-умолчанию 1.0). Это максимально допустимое соотношение между числом элементов и количеством корзин (count/buckets). Если при вставке очередного элемента среднее число элементов в корзинах превышает этот порог, число корзин (buckets) увеличивается и происходит рехеширование. Если заранее известно количество ключей, то можно сделать reserve и избежать лишних рехеширований при вставках.
 */
template <class Key, class Value, class Hash, class Equal>
float Unordered_Map<Key, Value, Hash, Equal>::Max_Load_Factor() const noexcept
{
    return _max_factor;
}

template <class Key, class Value, class Hash, class Equal>
void Unordered_Map<Key, Value, Hash, Equal>::Max_Load_Factor(float max_factor) noexcept
{
    _max_factor = max_factor;
}

// hash % buckets - вычисление принадлежности хэш-значения к бакету
template <class Key, class Value, class Hash, class Equal>
size_t Unordered_Map<Key, Value, Hash, Equal>::Bucket(const Key& key) const
{
    return Buckets_Count() > 0 ? static_cast<size_t>(Hash()(key) % Buckets_Count()) : 0;
}

// vector_index_size - кол-во элементов в списке в рамках одного bucket
template <class Key, class Value, class Hash, class Equal>
size_t Unordered_Map<Key, Value, Hash, Equal>::Bucket_Size(size_type index) const
{
    if (index >= _buckets.size())
        throw std::out_of_range("Index is out of range!");

    auto it = _buckets[index];
    if (it == Iterator())
        return 0;
    
    size_type count = 0;
    while (it.get() != _list.end() && it.bucket() == index)
    {
        ++it;
        ++count;
    }
    
    return count;
}

// vector_size - кол-во buckets
template <class Key, class Value, class Hash, class Equal>
size_t Unordered_Map<Key, Value, Hash, Equal>::Buckets_Count() const noexcept
{
    return _buckets.size();
}

template <class Key, class Value, class Hash, class Equal>
bool Unordered_Map<Key, Value, Hash, Equal>::Empty() const noexcept
{
    return Size() == 0;
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::size_type Unordered_Map<Key, Value, Hash, Equal>::Size() const noexcept
{
    return _size;
}

template <class Key, class Value, class Hash, class Equal>
void Unordered_Map<Key, Value, Hash, Equal>::Clear() noexcept
{
    _buckets.clear();
    _list.clear();
    _max_factor = 1.0;
    _size = 0;
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Iterator Unordered_Map<Key, Value, Hash, Equal>::Begin() noexcept
{
    return Iterator(_list.begin());
}

template <class Key, class Value, class Hash, class Equal>
Unordered_Map<Key, Value, Hash, Equal>::Iterator Unordered_Map<Key, Value, Hash, Equal>::End() noexcept
{
    return Iterator(_list.end());
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::Begin: переопределение
template <class Key, class Value, class Hash, class Equal>
auto Unordered_Map<Key, Value, Hash, Equal>::Begin() const noexcept -> Unordered_Map<Key, Value, Hash, Equal>::Const_Iterator
{
    return Iterator(_list.cbegin());
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::End: переопределение
template <class Key, class Value, class Hash, class Equal>
auto Unordered_Map<Key, Value, Hash, Equal>::End() const noexcept -> Unordered_Map<Key, Value, Hash, Equal>::Const_Iterator
{
    return Iterator(_list.crend());
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::CBegin: переопределение
template <class Key, class Value, class Hash, class Equal>
auto Unordered_Map<Key, Value, Hash, Equal>::CBegin() const noexcept -> Unordered_Map<Key, Value, Hash, Equal>::Const_Iterator
{
    return Begin();
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::CEnd: переопределение
template <class Key, class Value, class Hash, class Equal>
auto Unordered_Map<Key, Value, Hash, Equal>::CEnd() const noexcept -> Unordered_Map<Key, Value, Hash, Equal>::Const_Iterator
{
    return End();
}

#endif /* Unordered_Map_h */
