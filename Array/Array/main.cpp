#include "Array.h"

#include <algorithm>


class Example
{
public:
    
    Example() = default; // для Array обязательно должен быть конструктор по-умолчанию
    Example(const int number, const std::string& str) noexcept :
    _number(number),
    _str(str)
    {
        
    }
    
    Example(const Example& other) noexcept
    {
        *this = other;
    }
    
    Example(Example&& other) noexcept
    {
        *this = std::move(other);
    }
    
    Example& operator=(const Example& other) noexcept
    {
        if (this == &other)
            return *this;
        
        _number = other._number;
        _str = other._str;
        return *this;
    }
    
    Example& operator=(Example&& other) noexcept
    {
        if (this == &other)
            return *this;
        
        _number = std::exchange(other._number, 0);
        _str = std::move(other._str);
        return *this;
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
    // Array
    {
        std::cout << "Array" << std::endl;
        Array<int, 5> array;
        try
        {
            array.At(5);
        }
        catch (const std::out_of_range& exception)
        {
            std::cout << exception.what() << std::endl;
        }
        
        for (size_t i = 0, I = array.Size(); i < I; ++i)
            array[i] = (int)i;
        for (auto it = Begin(array); it != array.End(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        std::cout << "Array: reverse" << std::endl;
        for (auto it = array.RBegin(); it != array.REnd(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        
        [[maybe_unused]] auto data = array.Data();
        [[maybe_unused]] auto find = std::find(Begin(array), End(array), 3);
        
        [[maybe_unused]] auto front = array.Front();
        [[maybe_unused]] auto back = array.Back();
        [[maybe_unused]] auto compare1 = array.Begin() == array.End();
        [[maybe_unused]] auto compare2 = array.Begin() != array.End();
        array.Swap(array);
        array = array;
        array = std::move(array);
        Array<int, 5> array2(array);
        Array<int, 5> array3;
        array3 = array;
        Array<int, 5> array4(std::move(array2));
        Array<int, 5> array5;
        array5 = std::move(array3);
        array4.Swap(array5);
        Array<int, 3> array6 = {1, 2, 3};
        auto array7 = array6;
        array6 == array7;
        [[maybe_unused]] auto compare3 = (array6 == array7);
        [[maybe_unused]] auto compare4 = (array6 != array7);
        array6.At(1) = 5;
        [[maybe_unused]] auto compare5 = (array6 == array7);
        [[maybe_unused]] auto compare6 = (array6 != array7);
        array.Fill(6);
        
        std::cout << "Array: Fill" << std::endl;
        for (auto it = array.CBegin(); it != array.CEnd(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        
        Array<Example, 3> examples;
        examples[0] = Example(1, "number1");
        examples[1] = Example(2, "number2");
        examples[2] = Example(3, "number3");
        
        std::cout << "list example: " << std::endl;
        for (auto it = examples.Begin(); it != examples.End(); ++it)
            std::cout << *it;
        std::cout << std::endl;
        
        Array<Example, 3> examples_copy = examples;
        examples_copy[0] = Example(4, "number4");
        examples_copy[1] = Example(5, "number5");
        examples_copy[2] = Example(6, "number6");
        auto examples_move = std::move(examples);
        examples_copy.Swap(examples_move);
    }
    
    // massive
    {
        using namespace massive;
        
        int x[5] = {3,1,2,5,2};
        [[maybe_unused]] auto result1 = std::find(std::begin(x), std::end(x), 3);
#if defined (__APPLE__) || defined(__APPLE_CC__) || defined(__OSX__)
        [[maybe_unused]] auto result2 = std::find(RBegin(x), REnd(x), 3);
#endif
        std::sort(Begin(x), End(x));
    }
    
    return 0;
}
