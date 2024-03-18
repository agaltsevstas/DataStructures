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
        return os << "number: " << example._number << ", str: " << example._str << std::endl;
    }
    
private:
    int _number = 0;
    std::string _str;
};


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
        
        Test1<DEQUE::Stack<int>>();
        Test2<DEQUE::Stack<Example>>();
        
        std::cout << std::endl;
    }
    
    return 0;
}
