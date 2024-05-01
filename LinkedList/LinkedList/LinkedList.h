#ifndef LinkedList_h
#define LinkedList_h

#include <iostream>


/*
 Сайты:
 https://medium.com/geekculture/iterator-design-pattern-in-c-42caec84bfc
 */

template <class T>
class LinkedList
{
    class Iterator;
    friend class Iterator;
    using Const_Iterator = const Iterator;
    struct Node
    {
        template <typename ...Args>
        Node(Node* iNext, Args&& ...args) noexcept :
        next(iNext),
        value(std::forward<Args>(args)...)
        {
            
        }
        
        T value;
        Node* next = nullptr;
    };
    
public:
    LinkedList() = default;
    LinkedList(const std::initializer_list<T>& list);
    LinkedList(const Iterator& begin, const Iterator& end);
    LinkedList(const LinkedList& other);
    LinkedList(LinkedList&& other) noexcept;
    ~LinkedList();
    LinkedList& operator=(const LinkedList& other);
    LinkedList& operator=(LinkedList&& other) noexcept;
    bool operator==(const LinkedList& other);
    template <typename ...Args>
    decltype(auto) Emplace_Front(Args&& ...args);
    void Push_Front(const T& value);
    void Push_Front(T&& value);
    void Pop_Front();
    T& Front();
    const T& Front() const;
    void Swap(LinkedList& other) noexcept;
    bool Empty() const noexcept;
    size_t Size() const noexcept;
    void Reverse() noexcept;
    void Clear() noexcept;
    
    Iterator Begin() noexcept;
    Iterator End() noexcept;
    Const_Iterator Begin() const noexcept;
    Const_Iterator End() const noexcept;
    Const_Iterator CBegin() const noexcept;
    Const_Iterator CEnd() const noexcept;
    Iterator Insert_After(const Iterator& it, const T& value);
    Iterator Erase_After(const Iterator& it);
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream &os, const LinkedList<U>& list);
    
private:
    void Copy(const LinkedList& other)
    {
        Node* top_other = other._node;
        if (!top_other)
            return;
        
        Node* top = new Node(nullptr, other._node->value);
        _node = top;
        top_other = top_other->next;
        
        while (top_other)
        {
            top->next = new Node(nullptr, top_other->value);
            top = top->next;
            top_other = top_other->next;
        }
        
        _size = other._size;
    }
    
private:
    Node* _node = nullptr;
    size_t _size = 0;
};


template<typename T>
class LinkedList<T>::Iterator
{
    friend class LinkedList;
public:
    Iterator(const LinkedList& list, Node* node) :
        _list(list)
    {
        _node = node;
    }

    inline Iterator& operator++()
    {
        _node = _node ? _node->next : nullptr;
        return *this;
    }

    inline Iterator operator++(int)
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }

    inline T& operator*()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }
    
    inline const T& operator*() const
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }

    inline T* operator->()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return &_node->value;
    }
    
    inline const T* operator->() const
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return &_node->value;
    }

    inline Iterator& operator=(const Iterator& other)
    {
        if (&_list == &other._list)
            _node = other._node;
        return *this;
    }

    inline bool operator==(const Iterator& other) const
    {
        return &_list == &other._list && _node == other._node;
    }

    inline bool operator!=(const Iterator& other) const
    {
        return &_list != &other._list || _node != other._node;
    }

private:
    const LinkedList& _list;
    Node* _node = nullptr;
};


template <class T>
LinkedList<T>::LinkedList(const std::initializer_list<T>& list)
{
    for (auto it = list.begin(); it != list.end(); ++it)
        Push_Front(*it);
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>::Iterator& begin, const LinkedList<T>::Iterator& end)
{
    for (auto it = begin; it != end; ++it)
        Push_Front(*it);
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList& other)
{
    Copy(other);
}

template <class T>
LinkedList<T>::LinkedList(LinkedList&& other) noexcept
{
    _node = std::exchange(other._node, nullptr);
    _size = std::exchange(other._size, 0);
}

template <class T>
LinkedList<T>::~LinkedList()
{
    Clear();
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList& other)
{
    if (this == &other) // object = object
        return *this;
    
    Clear();
    Copy(other);
    
    return *this;
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(LinkedList&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    _node = std::exchange(other._node, nullptr);
    _size = std::exchange(other._size, 0);
    
    return *this;
}

template <class T>
bool LinkedList<T>::operator==(const LinkedList& other)
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

template <class T>
template <typename ...Args>
decltype(auto) LinkedList<T>::Emplace_Front(Args&& ...args)
{
    Node* node = new Node(_node, std::forward<Args>(args)...);
    _node = node;
    ++_size;
    return _node->value;
}

template <class T>
void LinkedList<T>::Push_Front(const T& value)
{
    Node* node = new Node(_node, value);
    _node = node;
    ++_size;
}

template <class T>
void LinkedList<T>::Push_Front(T&& value)
{
    Node* node = new Node(_node, std::move(value));
    _node = node;
    ++_size;
}

template <class T>
void LinkedList<T>::Pop_Front()
{
    if (Empty())
        throw std::runtime_error("LinkedList is empty");
    
    Node* tmp = _node;
    _node = _node->next;
    delete tmp;
    --_size;
}

template <class T>
T& LinkedList<T>::Front()
{
    if (!_node)
        throw std::runtime_error("LinkedList is null");
    return _node->value;
}

template <class T>
const T& LinkedList<T>::Front() const
{
    if (!_node)
        throw std::runtime_error("LinkedList is null");
    return _node->value;
}

template <class T>
void LinkedList<T>::Swap(LinkedList& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    std::swap(_node, other._node);
    std::swap(_size, other._size);
}

template <class T>
bool LinkedList<T>::Empty() const noexcept
{
    return Size() == 0;
}

template <class T>
size_t LinkedList<T>::Size() const noexcept
{
    return _size;
}

template <class T>
void LinkedList<T>::Reverse() noexcept
{
    Node* current = _node;
    Node *prev = NULL, *next = NULL;

    while (current)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    _node = prev;
}

template <class T>
void LinkedList<T>::Clear() noexcept
{
    while (_node)
    {
        Node* tmp = _node->next;
        delete _node;
        _node = tmp;
    }
    
    _node = nullptr;
    _size = 0;
}

template <class T>
LinkedList<T>::Iterator LinkedList<T>::Begin() noexcept
{
    return Iterator(*this, _node);
};

/// TODO
template <class T>
LinkedList<T>::Iterator LinkedList<T>::End() noexcept
{
    return Iterator(*this, nullptr); // return nullptr!!!
};

template <class T>
LinkedList<T>::Const_Iterator LinkedList<T>::Begin() const noexcept
{
    return Iterator(*this, _node);
}

/// TODO
template <class T>
LinkedList<T>::Const_Iterator LinkedList<T>::End() const noexcept
{
    return Iterator(*this, nullptr); // return nullptr!!!
}

template <class T>
LinkedList<T>::Const_Iterator LinkedList<T>::CBegin() const noexcept
{
    return Iterator(*this, _node);
}

template <class T>
LinkedList<T>::Const_Iterator LinkedList<T>::CEnd() const noexcept
{
    return Iterator(*this, nullptr); // return nullptr!!!
}

template <class T>
LinkedList<T>::Iterator LinkedList<T>::Insert_After(const Iterator& it, const T& value)
{
    if (!it._node)
        throw std::runtime_error("iterator is empty");
    
    Node* node = new Node(it._node, value);
    node->next = it._node->next;
    it._node->next = node;
    
    ++_size;
    return Iterator(*this, node);
}

template <class T>
LinkedList<T>::Iterator LinkedList<T>::Erase_After(const Iterator& it)
{
    if (!it._node)
        throw std::runtime_error("iterator is empty");
    
    Node* next = it._node->next;
    if (next->next)
    {
        it._node->next = next->next;
        delete next;
    }
    else
    {
        delete next;
        it._node->next = nullptr; // Без этого условия ошибка при раскрутке стека
    }
    --_size;
    return Iterator(*this, it._node->next);
}

template <class U>
std::ostream& operator<<(std::ostream &os, const LinkedList<U>& list)
{
    for (auto it = list.Begin(); it != list.End(); ++it)
            os << *it << " ";
    
    return os;
}


#endif /* LinkedList_h */
