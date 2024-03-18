#ifndef List_h
#define List_h

#include <iostream>

/*
 Сайты:
 https://medium.com/geekculture/iterator-design-pattern-in-c-42caec84bfc
 */

template <class T>
class List
{
    class Iterator;
    friend Iterator;
    struct Node
    {
        Node() = default;
        explicit Node(const T& iValue, Node* iPrev = nullptr, Node* iNext = nullptr) noexcept
        {
            value = iValue;
            prev = iPrev;
            next = iNext;
        }
        
        explicit Node(T&& iValue, Node* iPrev = nullptr, Node* iNext = nullptr) noexcept
        {
            value = std::move(iValue);
            prev = iPrev;
            next = iNext;
        }
        
        template <typename ...Args>
        explicit Node(Node* iPrev, Node* iNext, Args&& ...args) noexcept :
        prev(iPrev),
        next(iNext),
        value(std::forward<Args>(args)...)
        {
            
        }
        
        T value;
        Node* prev = nullptr;
        Node* next = nullptr;
    };
    
public:
    List() = default;
    List(const std::initializer_list<T>& list);
    List(const Iterator& begin, const Iterator& end);
    List(const List& other) noexcept;
    List(List&& other) noexcept;
    ~List();
    List& operator=(const List& other) noexcept;
    List& operator=(List&& other) noexcept;
    bool operator==(const List& other);
    template <typename ...Args>
    decltype(auto) Emplace_Front(Args&& ...args);
    template <typename ...Args>
    decltype(auto) Emplace_Back(Args&& ...args);
    void Push_Front(const T& value);
    void Push_Front(T&& value);
    void Push_Back(const T& value);
    void Push_Back(T&& value);
    void Pop_Front();
    void Pop_Back();
    T& Back();
    T& Front();
    void Swap(List& other) noexcept;
    bool Empty() const noexcept;
    size_t Size() const noexcept;
    void Reverse() noexcept;
    void Clear() noexcept;
    
    Iterator Begin() const noexcept;
    Iterator End() const noexcept;
    Iterator Insert(const Iterator& it, const T& value);
    Iterator Erase(const Iterator& it);
    Iterator Erase(const Iterator& begin, const Iterator& end);
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream &os, const List<U>& list);
    
private:
    void Copy(const List& other)
    {
        Node* begin_other = other._begin;
        if (!begin_other)
            return;
        _begin = new Node(begin_other->value, nullptr, nullptr);
        _end = _begin;
        begin_other = begin_other->next;

        while (begin_other)
        {
            Node* tmpEnd = _end;
            _end->next = new Node();
            _end = _end->next;
            _end->prev = tmpEnd;
            _end->next = nullptr;
            _end->value = begin_other->value;
            begin_other = begin_other->next;
        }
    }
    
private:
    Node* _begin = nullptr;
    Node* _end = nullptr;
    size_t _size = 0;
};


template<typename T>
class List<T>::Iterator
{
    friend class List;
public:
    Iterator(const List& list, Node* node) noexcept :
        _list(list)
    {

        _node = node;
    }

    inline Iterator& operator++() noexcept
    {
        _node = _node ? _node->next : nullptr;
        return *this;
    }

    inline Iterator operator++(int) noexcept
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }

    inline Iterator& operator--() noexcept
    {
        _node = _node ? _node->prev : nullptr;
        return *this;
    }

    inline Iterator operator--(int) noexcept
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }

    inline T& operator*()
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }

    inline T& operator->() const noexcept
    {
        if (!_node)
            throw std::runtime_error("iterator is null");
        return _node->value;
    }

    inline Iterator& operator=(const Iterator& other) noexcept
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
    const List& _list;
    Node* _node = nullptr;
};


template <class T>
List<T>::List(const std::initializer_list<T>& list)
{
    for (auto it = list.begin(); it != list.end(); ++it)
        Push_Back(*it);
}

template <class T>
List<T>::List(const List<T>::Iterator& begin, const List<T>::Iterator& end)
{
    for (auto it = begin; it != end; ++it)
        Push_Back(*it);
}

template <class T>
List<T>::List(const List& other) noexcept
{
    Copy(other);
}

template <class T>
List<T>::List(List&& other) noexcept
{
    _begin = std::exchange(other._begin, nullptr);
    _end = std::exchange(other._end, nullptr);
    _size = std::exchange(other._size, 0);
}

template <class T>
List<T>::~List()
{
    Clear();
}

template <class T>
List<T>& List<T>::operator=(const List& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    Clear();
    Copy(other);
    
    return *this;
}

template <class T>
List<T>& List<T>::operator=(List&& other) noexcept
{
    if (this == &other) // object = object
        return *this;
    
    _begin = std::exchange(other._begin, nullptr);
    _end = std::exchange(other._end, nullptr);
    _size = std::exchange(other._size, 0);
    
    return *this;
}

template <class T>
bool List<T>::operator==(const List& other)
{
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
decltype(auto) List<T>::Emplace_Front(Args&& ...args)
{
    if (_begin)
    {
        Node* node = new Node(nullptr, _begin, std::forward<Args>(args)...);
        _begin->prev = node;
        _begin = node;
    }
    else
        _begin = _end = new Node(nullptr, nullptr, std::forward<Args>(args)...);

    ++_size;
    return _begin->value;
}

template <class T>
template <typename ...Args>
decltype(auto) List<T>::Emplace_Back(Args&& ...args)
{
    if (_end)
    {
        Node* node = new Node(_end, nullptr, std::forward<Args>(args)...);
        _end->next = node;
        _end = node;
    }
    else
        _begin = _end = new Node(nullptr, nullptr, std::forward<Args>(args)...);

    ++_size;
    return _end->value;
}

template <class T>
void List<T>::Push_Front(const T& value)
{
    if (_begin)
    {
        Node* node = new Node(value, nullptr, _begin);
        _begin->prev = node;
        _begin = node;
    }
    else
        _begin = _end = new Node(value);

    ++_size;
}

template <class T>
void List<T>::Push_Front(T&& value)
{
    if (_begin)
    {
        Node* node = new Node(std::move(value), nullptr, _begin);
        _begin->prev = std::move(node);
        _begin = node;
    }
    else
    {
        _begin = _end = new Node(std::move(value));
    }

    ++_size;
}

template <class T>
void List<T>::Push_Back(const T& value)
{
    if (_end)
    {
        Node* node = new Node(value, _end, nullptr);
        _end->next = node;
        _end = node;
    }
    else
    {
        _begin = _end = new Node(value);
    }

    ++_size;
}

template <class T>
void List<T>::Push_Back(T&& value)
{
    if (_end)
    {
        Node* node = new Node(std::move(value), _end, nullptr);
        _end->next = node;
        _end = node;
    }
    else
    {
        _begin = _end = new Node(std::move(value));
    }
}

template <class T>
void List<T>::Pop_Front()
{
    if (Empty())
        throw std::runtime_error("List is empty");
    
    Node* tmp = _begin;
    _begin = _begin->next;
    delete tmp;
    --_size;
}

template <class T>
void List<T>::Pop_Back()
{
    if (Empty())
        throw std::runtime_error("List is empty");
    
    Node* tmp = _end;
    _end = _begin->prev;
    delete tmp;
    --_size;
}

template <class T>
T& List<T>::Front()
{
    if (!_begin)
        throw std::runtime_error("begin is null");
    return _begin->value;
}

template <class T>
T& List<T>::Back()
{
    if (!_end)
        throw std::runtime_error("end is null");
    return _end->value;
}

template <class T>
void List<T>::Swap(List& other) noexcept
{
    if (this == &other) // object.Swap(object)
        return;
    
    std::swap(_begin, other._begin);
    std::swap(_end, other._end);
    std::swap(_size, other._size);
}

template <class T>
bool List<T>::Empty() const noexcept
{
    return Size() == 0;
}

template <class T>
size_t List<T>::Size() const noexcept
{
    return _size;
}

template <class T>
void List<T>::Reverse() noexcept
{
    auto tmpEnd = _end;
    while (_begin != _end)
    {
      _end->prev = std::exchange(_end->next, _end->prev); // _end->prev = _end->next, _end->next = _end->prev
      _end = _end->next;
    }
    _begin->prev = std::exchange(_begin->next, _begin->prev); // _begin->prev = _begin->next, _begin->next = _begin->prev
    _begin = tmpEnd;
}

template <class T>
void List<T>::Clear() noexcept
{
    while (_begin)
    {
        Node* tmp = _begin->next;
        delete _begin;
        _begin = tmp;
    }
    
    _begin = nullptr;
    _end = nullptr;
    _size = 0;
}

template <class T>
List<T>::Iterator List<T>::Begin() const noexcept
{
    return Iterator(*this, _begin);
};

template <class T>
List<T>::Iterator List<T>::End() const noexcept
{
    return _end ? Iterator(*this, _end->next) : Iterator(*this, nullptr); // return nullptr!!!
};

template <class T>
List<T>::Iterator List<T>::Insert(const Iterator& it, const T& value)
{
    if (!it._node)
    {
        _begin = _end = new Node(value);
        return Iterator(*this, _end);
    }
    
    Node* tmp = it._node;
    Node* tmpPrev = it._node->prev;
    Node* tmpNext = it._node->next;
    Node* node = new Node(value);

    if (tmpPrev && tmpNext) // Вставка в середину
    {
        node->prev = tmpPrev;
        node->next = tmp;
        tmpPrev->next = node;
        tmp->prev = node;
    }
    else if (tmpNext) // Вставка в начало
    {
        node->next = tmp;
        tmp->prev = node;
    }
    else if (tmpPrev) // Вставка в конец
    {
        node->prev = tmpPrev;
        node->next = tmp;
        tmp->prev = node;
        tmpPrev->next = node;
    }
    else // Вставка после 1 элемента, если элемент 1
    {
        node->prev = tmp;
        tmp->next = node;
    }
    
    ++_size;
    return Iterator(*this, node);
}

template <class T>
List<T>::Iterator List<T>::Erase(const Iterator& it)
{
    if (!it._node)
        throw std::runtime_error("iterator is empty");
    else if (it._node == _begin)
    {
        Node* tmp = _begin;
        _begin = _begin->next;
        delete tmp;
        --_size;
        return Begin();
    }
    else if (it._node == _end)
    {
        Node* tmp = _end;
        _end = _begin->prev;
        delete tmp;
        --_size;
        return End();
    }
    
    Node* tmp = it._node;
    Node* tmpPrev = it._node->prev;
    Node* tmpNext = it._node->next;
    
    tmpPrev->next = tmpNext;
    tmpNext->prev = tmpPrev;
    delete tmp;
    --_size;
    return Iterator(*this, tmpNext);
}

template <class T>
List<T>::Iterator List<T>::Erase(const Iterator& begin, const Iterator& end)
{
    for (auto it = begin; it != end;)
        it = Erase(it);
    return end;
}

template <class U>
std::ostream& operator<<(std::ostream &os, const List<U>& list)
{
    for(auto it = list.Begin(); it != list.End(); ++it)
            os << *it << " ";
    
    return os;
}

#endif /* List_h */