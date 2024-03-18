#ifndef Linked_list_h
#define Linked_list_h

namespace linked_list
{
    template <class T>
    class Stack
    {
        struct Node
        {
            Node() = default;
            explicit Node(const T& iValue, Node* iPrev = nullptr) noexcept
            {
                value = iValue;
                prev = iPrev;
            }
            
            explicit Node(T&& iValue, Node* iPrev = nullptr) noexcept
            {
                value = std::move(iValue);
                prev = iPrev;
            }
            
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
        Stack(const Stack& other) noexcept;
        Stack(Stack&& other) noexcept;
        ~Stack();
        Stack& operator=(const Stack& other) noexcept;
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
            Node* top_other = other._node;
            if (!top_other)
                return;
            
            Node* top = new Node(other._node->value, nullptr);
            _node = top;
            top_other = top_other->prev;
            
            while (top_other)
            {
                top->prev = new Node(top_other->value, nullptr);
                top = top->prev;
                top_other = top_other->prev;
            }
            
            _size = other._size;
        }
    private:
        Node* _node = nullptr;
        size_t _size = 0;
    };

    template <class T>
    Stack<T>::Stack(const Stack& other) noexcept
    {
        Copy(other);
    }

    template <class T>
    Stack<T>::Stack(Stack&& other) noexcept
    {
        _node = std::exchange(other._node, nullptr);
        _size = std::exchange(other._size, 0);
    }

    template <class T>
    Stack<T>::~Stack()
    {
        while (_size-- > 0)
            Pop();
    }

    template <class T>
    Stack<T>& Stack<T>::operator=(const Stack& other) noexcept
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
        
        _node = std::exchange(other._node, nullptr);
        _size = std::exchange(other._size, 0);
        
        return *this;
    }

    // Помещает новый элемент на вершину стека. Элемент создается на месте, т.е. операции копирования или перемещения не выполняются. Конструктор элемента вызывается с теми же аргументами, что и функция.
    template <class T>
    template <typename ...Args>
    decltype(auto) Stack<T>::Emplace(Args&& ...args)
    {
        Node* node = new Node(_node, std::forward<Args>(args)...);
        _node = node;
        ++_size;
        return _node->value;
    }

    template <class T>
    void Stack<T>::Push(const T& value)
    {
        Node* node = new Node(value, _node);
        _node = node;
        ++_size;
    }

    template <class T>
    void Stack<T>::Push(T&& value)
    {
        Node* node = new Node(std::move(value), _node);
        _node = node;
        ++_size;
    }

    template <class T>
    T& Stack<T>::Top()
    {
        if (Empty())
            throw std::logic_error("Stack is empty");
        
        return _node->value;
    }

    template <class T>
    const T& Stack<T>::Top() const
    {
        if (Empty())
            throw std::logic_error("Stack is empty");
        
        return _node->value;
    }

    template <class T>
    void Stack<T>::Pop()
    {
        Node* top = _node->prev;
        delete _node;
        _node = top;
        --_size;
    }

    template <class T>
    void Stack<T>::Swap(Stack& other) noexcept
    {
        if (this == &other) // object.Swap(object)
            return;
        
        std::swap(_node, other._node);
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
