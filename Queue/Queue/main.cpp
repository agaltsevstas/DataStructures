#include <iostream>
#include <string>

#include "LinkedList.h"
#include "Deque.h"


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
    
    int GetNumber() const noexcept { return _number; }
    const std::string& GetValue() const noexcept { return _str; }
    
    friend std::ostream& operator<<(std::ostream &os, const Example& example)
    {
        return os << "number: " << example._number << ", str: " << example._str;
    }
    
private:
    int _number = 0;
    std::string _str;
};


template <class TQueue>
void Test1()
{
    int number = 1;
    TQueue Queue1;
    Queue1.Push(number);
    Queue1.Push(2);
    Queue1.Push(3);
    Queue1.Push(4);
    Queue1.Swap(Queue1);
    Queue1 = Queue1;
    Queue1 = std::move(Queue1);
    TQueue Queue2(Queue1);
    TQueue Queue3;
    Queue3 = Queue1;
    TQueue Queue4(std::move(Queue2));
    TQueue Queue5;
    Queue5 = std::move(Queue3);
    Queue4.Swap(Queue5);
    
    std::cout << "int: " << std::endl;
    while (!Queue1.Empty())
    {
        std::cout << "Front: " << Queue1.Front() << ", Back: " << Queue1.Back() << std::endl;
        Queue1.Pop();
    }
    std::cout << std::endl << std::endl;
};

template <class TQueue>
void Test2()
{
    TQueue example;
    example.Emplace(1, "number1");
    example.Emplace(2, "number2");
    example.Emplace(3, "number3");

    std::cout << "Example: " << std::endl;
    while (!example.Empty())
    {
        std::cout << "Front: " << example.Front() << "; Back: " << example.Back() << std::endl;
        example.Pop();
    }
}


#include <queue>
int main()
{
    // linked list
    {
        std::cout << "Queue on linked list" << std::endl;
        
        Test1<linked_list::Queue<int>>();
        Test2<linked_list::Queue<Example>>();
        
        std::cout << std::endl;
    }
    // deque
    {
        std::cout << "Queue on deque" << std::endl;
        
        Test1<DEQUE::Queue<int>>();
        Test2<DEQUE::Queue<Example>>();
        
        std::cout << std::endl;
    }
    
    return 0;
}
