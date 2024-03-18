#include "LinkedList.h"

#include <string>

/*
 Сайты:
 https://medium.com/geekculture/iterator-design-pattern-in-c-42caec84bfc
 */

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


int main()
{
    LinkedList<int> list;
    try
    {
        list.Erase_After(list.Begin());
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    try
    {
        [[maybe_unused]] auto front = list.Front();
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    try
    {
        list.Insert_After(list.Begin(), 0);
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    int number = 1;
    
    list.Push_Front(1);
    list.Insert_After(list.Begin(), 0);
    list.Clear();
    list.Push_Front(number);
    list.Insert_After(list.Begin(), 2);
    list.Insert_After(++list.Begin(), 3);
    [[maybe_unused]] auto front = list.Front();
    [[maybe_unused]] auto compare1 = list.Begin() == list.End();
    [[maybe_unused]] auto compare2 = list.Begin() != list.End();
    list.Swap(list);
    list = list;
    list = std::move(list);
    LinkedList<int> list2(list);
    LinkedList<int> list3;
    list3 = list;
    LinkedList<int> list4(std::move(list2));
    LinkedList<int> list5;
    list5 = std::move(list3);
    list4.Swap(list5);
    LinkedList<int> list6(list4.Begin(), list4.End());
    LinkedList<int> list7 = {1, 2, 3};
    
    std::cout << "list: ";
    for (auto it = list.Begin(); it != list.End(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    
    std::cout << "list reverse: ";
    list.Reverse();
    for (auto it = list.Begin(); it != list.End(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    
    [[maybe_unused]] auto after1 = list.Erase_After(list.Begin());
    [[maybe_unused]] auto after2 = list.Erase_After(list.Begin());
    
    LinkedList<Example> examples;
    [[maybe_unused]] auto emplace_front1 = examples.Emplace_Front(1, "number1");
    [[maybe_unused]] auto emplace_front2 = examples.Emplace_Front(2, "number2");
    [[maybe_unused]] auto emplace_front3 = examples.Emplace_Front(3, "number3");
    
    std::cout << "list example: " << std::endl;
    for (auto it = examples.Begin(); it != examples.End(); ++it)
        std::cout << *it;
    std::cout << std::endl;
    
    return 0;
}
