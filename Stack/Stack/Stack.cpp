#include <iostream>
#include <deque>
#include <string>

class Example
{
public:
    
    Example(const int number, const std::string& str) noexcept
    {
        _number = number;
        _str = str;
    }
    
    Example(int&& number, std::string&& str) noexcept
    {
        _number = std::exchange(number, 0);
        _str = std::move(str);
    }
    
    friend std::ostream& operator<<(std::ostream &os, const Example& example)
    {
        return os << "number: " << example._number << ", str: " << example._str << std::endl;
    }
    
private:
    int _number = 0;
    std::string _str;
};


namespace linked_list
{
    template <class T>
    class Stack
    {
        struct Node
        {
            Node() noexcept {}
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
            value(std::forward<Args>(args)...),
            prev(iPrev)
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
            if (!other._node)
                return;
            
            Node* top_other = other._node;
            Node* top = new Node(other._node->value, nullptr); // Для типов, у которых явно определен конструктор
            Node* top_copy = top;
            
            while (top_other)
            {
                top->prev = new Node(top_other->value, nullptr);
                top = top->prev;
                top_other = top_other->prev;
            }
            
            _node = top_copy;
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

namespace DEQUE
{
    template<typename T, class Container = std::deque<T>> // vector, list or deque
    class Stack
    {

    public:
        Stack() = default;
        ~Stack() = default;
        
        Stack(const Stack& other) noexcept
        {
            *this = other;
        }
        
        Stack(Stack&& other) noexcept
        {
            *this = std::move(other);
        }
        
        Stack& operator=(const Stack& other) noexcept
        {
            if (this == &other)
                return *this;
            
            _container = other._container;
            return *this;
        }
        
        Stack& operator=(Stack&& other) noexcept
        {
            if (this == &other)
                return *this;
            
            _container = std::move(other._container);
            return *this;
        }
        
        template<class... Args>
        decltype(auto) Emplace(Args&& ...args)
        {
            return _container.emplace_back(std::forward<Args>(args)...);
        }
        
        template<typename U>
        void Push(U&& value)
        {
            _container.emplace_back(std::forward<U>(value));
        }
        
        T& Top()
        {
            return _container.back();
        }
        
        const T& Top() const
        {
            return _container.back();
        }
        
        void Pop()
        {
            _container.pop_back();
        }
        
        void Swap(Stack& other) noexcept
        {
            std::swap(_container, other._container);
        }
        
        bool Empty() const noexcept
        {
            return _container.empty();
        }
        
        size_t Size() const noexcept
        {
            return _container.size();
        }
        
    private:
        Container _container;
    };
}

template <class TStack>
void Test1()
{
    int number = 1;
    TStack stack1;
    stack1.Push(number);
    stack1.Push(2);
    stack1.Push(3);
    stack1.Push(4);
    stack1.Swap(stack1);
    stack1 = stack1;
    stack1 = std::move(stack1);
    TStack stack2(stack1);
    TStack stack3;
    stack3 = stack1;
    TStack stack4(std::move(stack2));
    TStack stack5;
    stack5 = std::move(stack3);
    stack4.Swap(stack5);
    
    std::cout << "int: ";
    while (!stack1.Empty())
    {
        std::cout << stack1.Top() << " ";
        stack1.Pop();
    }
    std::cout << std::endl << std::endl;
};

template <class TStack>
void Test2()
{
    TStack example;
    example.Emplace(1, "number1");
    example.Emplace(2, "number2");
    example.Emplace(3, "number3");

    std::cout << "Example: " << std::endl;
    while (!example.Empty())
    {
        std::cout << example.Top();
        example.Pop();
    }
}

int main()
{
    // linked list
    {
        std::cout << "stack on linked list" << std::endl;
        Test1<linked_list::Stack<int>>();
        Test2<linked_list::Stack<Example>>();
        std::cout << std::endl;
    }
    // deque
    {
        std::cout << "stack on deque" << std::endl;
        using namespace DEQUE;
        
        Test1<DEQUE::Stack<int>>();
        Test2<DEQUE::Stack<Example>>();
        
        std::cout << std::endl;
    }
    
    return 0;
}
