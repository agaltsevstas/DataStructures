#include "List.h"

#include <string>

/*
 Сайты:
 https://medium.com/geekculture/iterator-design-pattern-in-c-42caec84bfc
 */

class Example
{
public:
    
    Example(const int number, const std::string& str) noexcept :
    _number(number),
    _str(str)
    {
        
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
    List<int> list;
    try
    {
        list.Erase(list.Begin());
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    try
    {
        [[maybe_unused]] auto front = list.Front();
        [[maybe_unused]] auto back = list.Back();
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    int number = 1;
    
    list.Insert(list.Begin(), 0);
    list.Clear();
    list.Emplace_Back(3);
    list.Push_Front(number);
    list.Insert(++list.Begin(), 2);
    list.Emplace_Back(4);
    list.Emplace_Back(5);
    list.Emplace_Back(6);
    [[maybe_unused]] auto front = list.Front();
    [[maybe_unused]] auto back = list.Back();
    [[maybe_unused]] auto compare1 = list.Begin() == list.End();
    [[maybe_unused]] auto compare2 = list.Begin() != list.End();
    list.Swap(list);
    list = list;
    list = std::move(list);
    List<int> list2(list);
    List<int> list3;
    list3 = list;
    List<int> list4(std::move(list2));
    List<int> list5;
    list5 = std::move(list3);
    list4.Swap(list5);
    List<int> list6(list4.Begin(), list4.End());
    List<int> list7 = {1, 2, 3};
    
    std::cout << "list: ";
    for (auto it = list.Begin(); it != list.End(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    
    std::cout << "list reverse: ";
    list.Reverse();
    for (auto it = list.Begin(); it != list.End(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    
    list.Erase(list.Begin());
    list.Erase(list.Begin(), list.End());
    
    List<Example> examples;
    examples.Emplace_Back(2, "number2");
    [[maybe_unused]] auto emplace_back = examples.Emplace_Back(3, "number3");
    [[maybe_unused]] auto emplace_front = examples.Emplace_Front(1, "number1");
    
    std::cout << "list example: " << std::endl;
    for (auto it = examples.Begin(); it != examples.End(); ++it)
        std::cout << *it;
    std::cout << std::endl;
    
    return 0;
}
