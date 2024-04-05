#ifndef LinkedList_h
#define LinkedList_h

namespace linked_list
{
    template <class T>
    class Queue
    {
        struct Node
        {
            template <typename ...Args>
            explicit Node(Args&& ...args) noexcept :
            value(std::forward<Args>(args)...)
            {
                
            }
            
            T value;
            Node* next = nullptr;
        };
        
    public:
        Queue() = default;
        Queue(const Queue& other);
        Queue(Queue&& other) noexcept;
        ~Queue();
        Queue& operator=(const Queue& other);
        Queue& operator=(Queue&& other) noexcept;
        template <typename ...Args>
        // Помещает новый элемент на вершину стека. Элемент создается на месте, т.е. операции копирования или перемещения не выполняются. Конструктор элемента вызывается с теми же аргументами, что и функция.
        decltype(auto) Emplace(Args&& ...args);
        void Push(const T& value);
        void Push(T&& value);
        T& Front();
        const T& Front() const;
        T& Back();
        const T& Back() const;
        void Pop();
        void Swap(Queue& other) noexcept;
        bool Empty() const noexcept;
        size_t Size() const noexcept;
        
    private:
        void Clear()
        {
            while (_size-- > 0)
                Pop();
        }
        
        void Copy(const Queue& other)
        {
            Node* begin_other = other._begin;
            Node* end_other = other._end;
            if (!begin_other)
                return;
            
            Node* begin = new Node(begin_other->value);
            _begin = begin;
            begin_other = begin_other->next;
            
            while (begin_other)
            {
                begin->next = new Node(begin_other->value);
                begin = begin->next;
                if (begin_other == end_other)
                    _end = begin;
                begin_other = begin_other->next;
            }
            
            _size = other._size;
        }
    private:
        Node* _begin = nullptr;
        Node* _end = nullptr;
        size_t _size = 0;
    };

    template <class T>
    Queue<T>::Queue(const Queue& other)
    {
        Copy(other);
    }

    template <class T>
    Queue<T>::Queue(Queue&& other) noexcept
    {
        _begin = std::exchange(other._begin, nullptr);
        _end = std::exchange(other._end, nullptr);
        _size = std::exchange(other._size, 0);
    }

    template <class T>
    Queue<T>::~Queue()
    {
        while (_size-- > 0)
            Pop();
    }

    template <class T>
    Queue<T>& Queue<T>::operator=(const Queue& other)
    {
        if (this == &other) // object = object
            return *this;
        
        Clear();
        Copy(other);
        
        return *this;
    }

    template <class T>
    Queue<T>& Queue<T>::operator=(Queue&& other) noexcept
    {
        if (this == &other) // object = object
            return *this;
        
        _begin = std::exchange(other._begin, nullptr);
        _end = std::exchange(other._end, nullptr);
        _size = std::exchange(other._size, 0);
        
        return *this;
    }

    // Помещает новый элемент на вершину стека. Элемент создается на месте, т.е. операции копирования или перемещения не выполняются. Конструктор элемента вызывается с теми же аргументами, что и функция.
    template <class T>
    template <typename ...Args>
    decltype(auto) Queue<T>::Emplace(Args&& ...args)
    {
        if (_begin && _end)
        {
            _end->next = new Node(std::forward<Args>(args)...);
            _end = _end->next;
        }
        else
        {
            _begin = _end = new Node(std::forward<Args>(args)...);
        }
        return _end->value;
    }

    template <class T>
    void Queue<T>::Push(const T& value)
    {
        if (_begin && _end)
        {
            _end->next = new Node(value);
            _end = _end->next;
        }
        else
        {
            _begin = _end = new Node(value);
        }
        
        ++_size;
    }

    template <class T>
    void Queue<T>::Push(T&& value)
    {
        if (_begin && _end)
        {
            _end->next = new Node(std::move(value));
            _end = _end->next;
        }
        else
        {
            _begin = _end = new Node(std::move(value));
        }
        
        ++_size;
    }

    template <class T>
    T& Queue<T>::Front()
    {
        if (Empty())
            throw std::logic_error("Queue is empty");
        
        return _begin->value;
    }

    template <class T>
    const T& Queue<T>::Front() const
    {
        if (Empty())
            throw std::logic_error("Queue is empty");
        
        return _begin->value;
    }

    template <class T>
    T& Queue<T>::Back()
    {
        if (Empty())
            throw std::logic_error("Queue is empty");
        
        return _end->value;
    }

    template <class T>
    const T& Queue<T>::Back() const
    {
        if (Empty())
            throw std::logic_error("Queue is empty");
        
        return _end->value;
    }

    template <class T>
    void Queue<T>::Pop()
    {
        if (Empty())
            throw std::logic_error("Queue is empty");
        
        Node* begin = _begin;
        _begin = _begin->next;
        delete begin;
        --_size;
    }

    template <class T>
    void Queue<T>::Swap(Queue& other) noexcept
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_begin, other._begin);
        std::swap(_end, other._end);
        std::swap(_size, other._size);
    }

    template <class T>
    bool Queue<T>::Empty() const noexcept
    {
        return Size() == 0;
    }

    template <class T>
    size_t Queue<T>::Size() const noexcept
    {
        return _size;
    }
}

#endif /* LinkedList_h */
