#ifndef Map_h
#define Map_h

#include <iostream>
#include <functional>

/*
 Видео: https://www.youtube.com/watch?v=oYyEqfi_4fo&ab_channel=selfedu
 */

/*
 Сайты: https://github.com/VladimirBalun/Algorithms/blob/master/DataStructures/BinaryTree.cpp
        https://github.com/MikhailPimenov/s21_containers/blob/develop/src/s21_tree.h
        https://github.com/Nikhil14/Map/blob/master/map.hpp
        https://github.com/pradykaushik/Map-Container/blob/master/Map.hpp
 */


template <typename Key, typename Value>
inline void swap(std::pair<const Key, Value>& lhs, std::pair<const Key, Value>& rhs) noexcept
{
    std::swap(*const_cast<Key*>(&lhs.first), *const_cast<Key*>(&rhs.first));
    std::swap(lhs.second, rhs.second);
}

template <typename T>
struct Less
{
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs < rhs;
    }
};

// Красно-черное дерево
template <class Key,
          class Value,
          class Compare = Less<Key>>
class Map
{
    using size_type = size_t;
    using value_type = std::pair<const Key, Value>; // ключ не может меняться, поэтому const
    using const_value_type = const std::pair<const Key, Value>;
    
private:
    struct Node
    {
        Node() :
        value({Key(), Value()})
        {
            
        }
        
        template <typename ...Args>
        Node(Node* iParent, Node* iLeftChild, Node* iRightChild, Args&& ...args) :
        parent(iParent),
        leftChild(iLeftChild),
        rightChild(iRightChild),
        value(std::forward<Args>(args)...)
        {
            
        }
        
        value_type value; // ключ не может меняться, поэтому const
        Node* parent = nullptr;
        Node* leftChild = nullptr;
        Node* rightChild = nullptr;
    };
    
public:
    class Iterator;
    friend class Iterator;
    using Const_Iterator = const Iterator;
    
    class ReverseIterator;
    friend class ReverseIterator;
    using Const_ReverseIterator = const ReverseIterator;
    
    Map() = default;
    ~Map() = default;
    
    Map(const std::initializer_list<value_type>& map) noexcept;
    Map(const Map& other);
    Map(Map&& other) noexcept;
    Map& operator=(const Map& other);
    Map& operator=(Map&& other) noexcept;
    bool operator==(const Map& other) const noexcept;
    bool operator!=(const Map& other) const noexcept;
    // Менее эффективно - создается значение по умолчанию, а потом происходит присвоение
    Value& operator[](const Key& key);
    // Создается ключ со значением по умолчанию
    const Value& operator[](const Key& key) const;
    Value& At(const Key& key);
    const Value& At(const Key& key) const;
    
    template <typename ...Args>
    std::pair<Iterator, bool> Emplace(Args&& ...args);
    std::pair<Iterator, bool> Insert(const_value_type& element);
    // TODO: кладет рядом с итератором, если значения не сильно отличаются, время стремится -> Time: O(1)
    std::pair<Iterator, bool> Insert(Const_Iterator it, const_value_type& element);
    Iterator Find(const Key& key) const noexcept;
    size_type Count(const Key& key) const noexcept;
    bool Contains(const Key& key) const noexcept;
    Iterator Erase(const Key& key);
    Iterator Erase(Const_Iterator it);
    Iterator Erase(Const_Iterator begin, Const_Iterator end);
    
    void Swap(Map& other) noexcept;
    size_type Depth() const noexcept;
    bool Empty() const noexcept;
    size_type Size() const noexcept;
    void Clear() noexcept;
    
    Iterator Begin() noexcept;
    Iterator End() noexcept;
    Const_Iterator Begin() const noexcept;
    Const_Iterator End() const noexcept;
    Const_Iterator CBegin() const noexcept;
    Const_Iterator CEnd() const noexcept;
    
    // Чтение слева направо
    std::vector<value_type> InorderTraversal()
    {
        std::function<void(Node* node, std::vector<value_type>& result)> traverse;
        traverse = [&](Node* node, std::vector<value_type>& result)
        {
            if (node)
            {
                traverse(node->leftChild, result);
                result.emplace_back(node->value);
                traverse(node->rightChild, result);
            }
        };
        
        std::vector<value_type> result;
        traverse(_root, result);
        return result;
    }
    
    // Чтение сверху вниз
    std::vector<value_type> PreorderTraversal()
    {
        std::function<void(Node* node, std::vector<value_type>& result)> traverse;
        traverse = [&](Node* node, std::vector<value_type>& result)
        {
            if (node)
            {
                result.emplace_back(node->value);
                traverse(node->leftChild, result);
                traverse(node->rightChild, result);
            }
        };
        
        std::vector<value_type> result;
        traverse(_root, result);
        return result;
    }
    
    // Чтение снизу вверх
    std::vector<value_type> PostorderTraversal()
    {
        std::function<void(Node* node, std::vector<value_type>& result)> traverse;
        traverse = [&](Node* node, std::vector<value_type>& result)
        {
            if (node)
            {
                traverse(node->leftChild, result);
                traverse(node->rightChild, result);
                result.emplace_back(node->value);
            }
        };
        
        std::vector<value_type> result;
        traverse(_root, result);
        return result;
    }
    
private:
    Node* _root = nullptr;
    Node* _begin = nullptr;
    Node* _end = new Node();
    size_type _size = 0u;
};

template <class Key, class Value, class Compare>
class Map<Key, Value, Compare>::Iterator
{
    friend class Map;
public:
    Iterator(const Map& map, Node* node) :
    _map(map)
    {
        _node = node;
    }

    inline Iterator& operator++()
    {
        if (_node == nullptr)
        {
            _node = _map._begin;
            
            if (!_node)
                throw std::runtime_error("map is empty");
        }
        else if (_node->rightChild)
        {
            _node = _node->rightChild;
            while (_node->leftChild)
                _node = _node->leftChild;
        }
        else
        {
            Node* node = _node->parent;
            while (node && _node == node->rightChild)
            {
                _node = node;
                node = node->parent;
            }
            
            _node = node;
        }
        
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
        if (_node == nullptr)
        {
            _node = _map._end;
            
            if (!_node)
                throw std::runtime_error("map is empty");
        }
        else if (_node->leftChild)
        {
            _node = _node->leftChild;
            while (_node->rightChild)
                _node = _node->rightChild;
        }
        else
        {
            if (_node->parent)
                _node = _node->parent;
            else
                _node = _map._end;
        }
        
        return *this;
    }

    inline Iterator operator--(int)
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }

    inline value_type& operator*()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }
    
    inline const value_type& operator*() const
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }

    inline value_type* operator->()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return &_node->value;
    }
    
    inline const value_type* operator->() const
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return &_node->value;
    }

    inline Iterator& operator=(const Iterator& other)
    {
        if (&_map == &other._map)
            _node = other._node;
        return *this;
    }

    inline bool operator==(const Iterator& other) const
    {
        return &_map == &other._map && _node == other._node;
    }

    inline bool operator!=(const Iterator& other) const
    {
        return &_map != &other._map || _node != other._node;
    }
    
private:
    const Map& _map;
    Node* _node = nullptr;
};


template <class Key, class Value, class Compare>
class Map<Key, Value, Compare>::ReverseIterator
{
    friend class Map;
public:
    ReverseIterator(const Map& map, Node* node) :
    _map(map)
    {
        _node = node;
    }

    inline ReverseIterator& operator++()
    {
        _node = --Iterator(_map, _node);
        return *this;
    }

    inline ReverseIterator operator++(int)
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }

    inline ReverseIterator& operator--()
    {
        _node = ++Iterator(_map, _node);
        return *this;
    }

    inline ReverseIterator operator--(int)
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }

    inline value_type& operator*()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }
    
    inline const value_type& operator*() const
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }

    inline value_type* operator->()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return &_node->value;
    }
    
    inline const value_type* operator->() const
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return &_node->value;
    }

    inline ReverseIterator& operator=(const ReverseIterator& other)
    {
        if (&_map == &other._map)
            _node = other._node;
        return *this;
    }

    inline bool operator==(const ReverseIterator& other) const
    {
        return &_map == &other._map && _node == other._node;
    }

    inline bool operator!=(const ReverseIterator& other) const
    {
        return &_map != &other._map || _node != other._node;
    }
    
private:
    const Map& _map;
    Node* _node = nullptr;
};


template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map(const std::initializer_list<value_type>& map) noexcept
{
    for (const auto &elem : map)
        Insert(elem);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map(const Map& other)
{
    for (auto it = other.Begin(); it != other.End(); ++it)
        Insert(*it);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map(Map&& other) noexcept
{
    _root = std::exchange(other._root, nullptr);
    _begin = std::exchange(other._begin, nullptr);
    _end = std::exchange(other._end, nullptr);
    _size = std::exchange(other._size, 0);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>& Map<Key, Value, Compare>::operator=(const Map& other)
{
    if (this == &other) // object = object
        return *this;
    
    Clear();
    for (auto it = other.Begin(); it != other.End(); ++it)
        Insert(*it);
    
    return *this;
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>& Map<Key, Value, Compare>::operator=(Map&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    _root = std::exchange(other._root, nullptr);
    _begin = std::exchange(other._begin, nullptr);
    _end = std::exchange(other._end, nullptr);
    _size = std::exchange(other._size, 0);
    
    return *this;
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::operator==(const Map& other) const noexcept
{
    if (this == &other) // object = object
        return true;
    
    if (Size() != other.Size())
        return false;
    
    for (auto it = Begin(), it_other = other.Begin(); it != End(); ++it, ++it_other)
    {
        if (*it != *it_other)
            return false;
    }
    
    return true;
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::operator!=(const Map& other) const noexcept
{
    return !(*this == other);
}

// Менее эффективно - создается значение по умолчанию, а потом происходит присвоение
template <class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::operator[](const Key& key)
{
    auto [it, flag] = Emplace(std::move(std::make_pair(key, Value())));
    return it->second;
}

// Создается ключ со значением по умолчанию
template <class Key, class Value, class Compare>
const Value& Map<Key, Value, Compare>::operator[](const Key& key) const
{
    auto [it, flag] = Emplace(std::move(std::make_pair(key, Value())));
    return it->data.second;
}

template <class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::At(const Key& key)
{
    auto it = Find(key);
    if (it == End())
        throw std::runtime_error("Key is not exist!");
    
    return it->second;
}

template <class Key, class Value, class Compare>
const Value& Map<Key, Value, Compare>::At(const Key& key) const
{
    auto it = Find(key);
    if (it == End())
        throw std::runtime_error("Key is not exist!");
      
    return it->second;
}

template <class Key, class Value, class Compare>
template <typename ...Args>
std::pair<typename Map<Key, Value, Compare>::Iterator, bool> Map<Key, Value, Compare>::Emplace(Args&& ...args)
{
    auto value = value_type(std::forward<Args>(args)...); // В случае exception элемент не добавится
    
    if (Empty())
    {
        _root = new Node(nullptr, nullptr, _end, std::move(value));
        _end->parent = _root;
        _begin = _root;
        ++_size;
        return std::make_pair(Iterator(*this, _root), true);
    }
    
    std::function<std::pair<Iterator, bool>(Node* node)> AddNode;
    AddNode = [&](Node* node)->std::pair<Iterator, bool>
    {
        if (Compare()(value.first, node->value.first))
        {
            if (node->leftChild == nullptr)
            {
                node->leftChild = new Node(node, nullptr, nullptr, std::move(value));
                node = node->leftChild;
                if (node->parent == _begin)
                    _begin = node;
                return std::make_pair(Iterator(*this, node), true);
            }
            else
            {
                AddNode(node->leftChild);
            }
        }
        else if (Compare()(node->value.first, value.first))
        {
            if (node->rightChild == _end)
            {
                node->rightChild = new Node(node, nullptr, _end, std::move(value));
                node = node->rightChild;
                _end->parent = node;
                return std::make_pair(Iterator(*this, node), true);
            }
            else if (node->rightChild == nullptr)
            {
                node->rightChild = new Node(node, nullptr, nullptr, std::move(value));
                node = node->rightChild;
                return std::make_pair(Iterator(*this, node), true);
            }
            else
            {
                AddNode(node->rightChild);
            }
        }
        
        return {Iterator(*this, node), false};
    };
    
    auto result = AddNode(_root);

    ++_size;
    return result;
}

template <class Key, class Value, class Compare>
std::pair<typename Map<Key, Value, Compare>::Iterator, bool> Map<Key, Value, Compare>::Insert(const_value_type& element)
{
    return Emplace(std::move(element));
}

// TODO: кладет рядом с итератором, если значения не сильно отличаются, время стремится -> Time: O(1)
template <class Key, class Value, class Compare>
std::pair<typename Map<Key, Value, Compare>::Iterator, bool> Map<Key, Value, Compare>::Insert(Const_Iterator it, const_value_type& element)
{
    
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Find(const Key& key) const noexcept
{
    std::function<Iterator(Node* node)> FindNode;
    FindNode = [&](Node* node)->Iterator
    {
        if (node)
        {
            if (Compare()(key, node->value.first))
            {
                return FindNode(node->leftChild);
            }
            else if (Compare()(node->value.first, key))
            {
                return FindNode(node->rightChild);
            }
            else
            {
                return Iterator(*this, node);
            }
        }
        
        return Iterator(*this, _end);
    };
    
    return FindNode(_root);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::size_type Map<Key, Value, Compare>::Count(const Key& key) const noexcept
{
    return Find(key) != End() ? 1u : 0u;
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::Contains(const Key& key) const noexcept
{
    return Find(key) != End();
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Erase(const Key& key)
{
    Iterator it = Find(key);
    return it == End() ? it : Erase(it);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Erase(Const_Iterator it)
{
    if (it == End())
        return it;
    
    Node* node = it._node;
    if (node == _root)
    {
        if (node->leftChild && node->rightChild && node->rightChild != _end)
        {
            node = node->rightChild;
            while (node->leftChild)
                node = node->leftChild;
            
            auto parent = node->parent;
            if (node->rightChild)
            {
                node->rightChild->parent = parent;
                if (parent->leftChild == node)
                {
                    parent->leftChild = node->rightChild;
                    node->rightChild->parent = parent;
                }
                else if (parent->rightChild == node)
                {
                    parent->rightChild = node->rightChild;
                    node->rightChild->parent = parent;
                }
                
                swap(it._node->value, node->value);
            }
            else
            {
                if (parent->leftChild == node)
                    parent->leftChild = nullptr;
                else if (parent->rightChild == node)
                    parent->rightChild = nullptr;
                
                swap(it._node->value, node->value);
            }
        }
        else if (node->leftChild)
        {
            _root = node->leftChild;
            _root->parent = nullptr;
            Node* end = _root;
            while (end->rightChild)
                end = end->rightChild;
            
            end->rightChild = _end;
            _end->parent = end;
        }
        else if (node->rightChild && node->rightChild != _end)
        {
            _root = node->rightChild;
            _root->parent = nullptr;
            _begin = _root;
            while (_begin->leftChild)
                _begin = _begin->leftChild;
        }
        else
        {
            _begin = nullptr;
            _root = nullptr;
        }
    }
    else if (node->leftChild && node->rightChild && node->rightChild != _end)
    {
        node = node->rightChild;
        while (node->leftChild)
            node = node->leftChild;
        
        auto parent = node->parent;
        if (node->rightChild)
        {
            node->rightChild->parent = parent;
            if (parent->leftChild == node)
            {
                parent->leftChild = node->rightChild;
                node->rightChild->parent = parent;
            }
            else if (parent->rightChild == node)
            {
                parent->rightChild = node->rightChild;
                node->rightChild->parent = parent;
            }
            
            swap(it._node->value, node->value);
        }
        else
        {
            if (parent->leftChild == node)
                parent->leftChild = nullptr;
            else if (parent->rightChild == node)
                parent->rightChild = nullptr;
            
            swap(it._node->value, node->value);
        }
    }
    else if (node->leftChild)
    {
        auto parent = node->parent;
        if (parent->leftChild == node)
        {
            parent->leftChild = node->leftChild;
            node->leftChild->parent = parent;
        }
        else if (parent->rightChild == node)
        {
            parent->rightChild = node->leftChild;
            node->leftChild->parent = parent;
            if (node->rightChild == _end)
            {
                Node* end = node->leftChild;
                while (end->rightChild)
                    end = end->rightChild;
                
                end->rightChild = _end;
                _end->parent = end;
            }
        }
    }
    else if (node->rightChild && node->rightChild != _end)
    {
        auto parent = node->parent;
        if (parent->leftChild == node)
        {
            parent->leftChild = node->rightChild;
            node->rightChild->parent = parent;
        }
        else if (parent->rightChild == node)
        {
            parent->rightChild = node->rightChild;
            node->rightChild->parent = parent;
        }
        
        if (node == _begin)
            _begin = parent;
    }
    else
    {
        auto parent = node->parent;
        if (parent->leftChild == node)
            parent->leftChild = nullptr;
        else if (parent->rightChild == node)
        {
            if (node->rightChild == _end)
            {
                parent->rightChild = _end;
                _end->parent = parent;
            }
            else
                parent->rightChild = nullptr;
        }
        
        if (node == _begin)
            _begin = parent;
    }
    
    auto newIt = ++Iterator(*this, node);
    delete node;
    --_size;
    return newIt;
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Erase(Const_Iterator begin, Const_Iterator end)
{
    auto it = begin;
    while (it != end)
        it = Erase(it);
    return it;
}

template <class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Swap(Map& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    std::swap(_root, other._root);
    std::swap(_begin, other._begin);
    std::swap(_end, other._end);
    std::swap(_size, other._size);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::size_type Map<Key, Value, Compare>::Depth() const noexcept
{
    std::function<size_type(Node* node)> Depth;
    Depth = [&](Node* node)->size_type
    {
        if (!node)
            return 0u;
        
        size_type left_depth = Depth(node->leftChild);
        size_type right_depth = Depth(node->rightChild);
        
        return left_depth > right_depth ? ++left_depth : ++right_depth;
    };
    
    return Depth(_root);
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::Empty() const noexcept
{
    return Size() == 0;
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::size_type Map<Key, Value, Compare>::Size() const noexcept
{
    return _size;
}

template <class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Clear() noexcept
{
    std::function<void(Node* node)> Clear;
    Clear = [&](Node* node)
    {
        if (node)
        {
            Clear(node->leftChild);
            Clear(node->rightChild);
            delete node;
            node = nullptr;
        }
    };

    Clear(_root);
    
    _root = nullptr;
    _begin = nullptr;
    _size = 0;
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Begin() noexcept
{
    return _begin ? Iterator(*this, _begin) : Iterator(*this, _end);
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::End() noexcept
{
    return Iterator(*this, _end);
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::Begin: переопределение
template <class Key, class Value, class Compare>
auto Map<Key, Value, Compare>::Begin() const noexcept -> Map<Key, Value, Compare>::Const_Iterator
{
    return _begin ? Const_Iterator(*this, _begin) : Const_Iterator(*this, _end);
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::End: переопределение
template <class Key, class Value, class Compare>
auto Map<Key, Value, Compare>::End() const noexcept -> Map<Key, Value, Compare>::Const_Iterator
{
    return Const_Iterator(*this, _end);
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::CBegin: переопределение
template <class Key, class Value, class Compare>
auto Map<Key, Value, Compare>::CBegin() const noexcept -> Map<Key, Value, Compare>::Const_Iterator
{
    return Begin();
}

// Обход ошибки: C2373	Map<Key, Value, Compare>::CEnd: переопределение
template <class Key, class Value, class Compare>
auto Map<Key, Value, Compare>::CEnd() const noexcept -> Map<Key, Value, Compare>::Const_Iterator
{
    return End();
}

#endif /* Map_h */
