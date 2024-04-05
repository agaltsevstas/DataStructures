#ifndef Linked_list_h
#define Linked_list_h

namespace linked_list
{
    template <class T>
    class Stack
    {
        struct Node
        {
            template <typename ...Args>
            explicit Node(Node* iPrev, Args&& ...args) noexcept :
            prev(iPrev),
            value(std::forward<Args>(args)...)
            {
                
            }
            
            T value;
            Node* prev = nullptr;
        };
        
    public:
        Stack() = default;
        Stack(const Stack& other);
        Stack(Stack&& other) noexcept;
        ~Stack();
        Stack& operator=(const Stack& other);
        Stack& operator=(Stack&& other) noexcept;
        template <typename ...Args>
        // Помещает новый элемент на вершину стека. Элемент создается на месте, т.е. операции копирования или перемещения не выполняются. Конструктор элемента вызывается с теми же аргументами, что и функция.
        decltype(auto) Emplace(Args&& ...args);
        void Push(const T& value);
        void Push(T&& value);
        T& Top();
        const T& Top() const;
        void Pop();
        void Swap(Stack& other) noexcept;
        bool Empty() const noexcept;
        size_t Size() const noexcept;
        
    private:
        void Clear()
        {
            while (_size-- > 0)
                Pop();
        }
        
        void Copy(const Stack& other)
        {
            Node* top_other = other._head;
            if (!top_other)
                return;
            
            Node* top = new Node(nullptr, top_other->value);
            _head = top;
            top_other = top_other->prev;
            
            while (top_other)
            {
                top->prev = new Node(nullptr, top_other->value);
                top = top->prev;
                top_other = top_other->prev;
            }
            
            _size = other._size;
        }
    private:
        Node* _head = nullptr;
        size_t _size = 0;
    };

    template <class T>
    Stack<T>::Stack(const Stack& other)
    {
        Copy(other);
    }

    template <class T>
    Stack<T>::Stack(Stack&& other) noexcept
    {
        _head = std::exchange(other._head, nullptr);
        _size = std::exchange(other._size, 0);
    }

    template <class T>
    Stack<T>::~Stack()
    {
        while (_size-- > 0)
            Pop();
    }

    template <class T>
    Stack<T>& Stack<T>::operator=(const Stack& other)
    {
        if (this == &other) // object = object
            return *this;
        
        Clear();
        Copy(other);
        
        return *this;
    }

    template <class T>
    Stack<T>& Stack<T>::operator=(Stack&& other) noexcept
    {
        if (this == &other) // object = object
            return *this;
        
        _head = std::exchange(other._head, nullptr);
        _size = std::exchange(other._size, 0);
        
        return *this;
    }

    // Помещает новый элемент на вершину стека. Элемент создается на месте, т.е. операции копирования или перемещения не выполняются. Конструктор элемента вызывается с теми же аргументами, что и функция.
    template <class T>
    template <typename ...Args>
    decltype(auto) Stack<T>::Emplace(Args&& ...args)
    {
        Node* node = new Node(_head, std::forward<Args>(args)...);
        _head = node;
        ++_size;
        return _head->value;
    }

    template <class T>
    void Stack<T>::Push(const T& value)
    {
        Node* node = new Node(_head, value);
        _head = node;
        ++_size;
    }

    template <class T>
    void Stack<T>::Push(T&& value)
    {
        Node* node = new Node(_head, std::move(value));
        _head = node;
        ++_size;
    }

    template <class T>
    T& Stack<T>::Top()
    {
        if (Empty())
            throw std::logic_error("Stack is empty");
        
        return _head->value;
    }

    template <class T>
    const T& Stack<T>::Top() const
    {
        if (Empty())
            throw std::logic_error("Stack is empty");
        
        return _head->value;
    }

    template <class T>
    void Stack<T>::Pop()
    {
        if (Empty())
            throw std::logic_error("Stack is empty");
        
        Node* top = _head->prev;
        delete _head;
        _head = top;
        --_size;
    }

    template <class T>
    void Stack<T>::Swap(Stack& other) noexcept
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_head, other._head);
        std::swap(_size, other._size);
    }

    template <class T>
    bool Stack<T>::Empty() const noexcept
    {
        return Size() == 0;
    }

    template <class T>
    size_t Stack<T>::Size() const noexcept
    {
        return _size;
    }
}

#endif /* Linked_list_h */
