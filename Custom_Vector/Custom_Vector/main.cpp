#include "Custom_VectorBool.h"


/*
 Лекция: https://www.youtube.com/watch?v=kUqXNSgdd5A&ysclid=lu8lgbqu7g137468251
 Сайты: https://github.com/jakjan95/vector/blob/main/include/vector.hpp
        https://github.com/chetvertakov/Vector/blob/master/vector.h
        https://github.com/skayfish/Vector/blob/master/vector.h
        https://github.com/MaximShichanin/Vector/blob/master/vector.h
 */


class Example
{
public:
    
    Example() = default; // для Vector обязательно должен быть конструктор по-умолчанию
    Example(const int number, const std::string& str) noexcept
    {
        _number = number;
        _str = str;
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
        _number = other._number;
        _str = other._str;
        return *this;
    }
    
    Example& operator=(Example&& other) noexcept
    {
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
    // Vector
    {
        std::cout << "Vector: bool" << std::endl;
        Custom_Vector<bool> vector_bool(10, false);
        vector_bool[5] = true;
        for (size_t i = 0; i < vector_bool.Size(); ++i)
            std::cout << vector_bool[i] << " ";
        std::cout << std::endl;
        
        Custom_Vector<int> vector;
        [[maybe_unused]] auto capacity1 = vector.Capacity();
        vector.Reserve(5);
        [[maybe_unused]] auto capacity2 = vector.Capacity();
        vector.Shrink_To_Fit();
        [[maybe_unused]] auto capacity3 = vector.Capacity();
        
        int number = 1;
        vector.Insert(vector.Begin(), 0);
        vector.Clear();
        try
        {
            vector.Erase(vector.Begin());
        }
        catch (const std::range_error& exception)
        {
            std::cout << exception.what() << std::endl;
        }
        try
        {
            vector.At(5);
        }
        catch (const std::out_of_range& exception)
        {
            std::cout << exception.what() << std::endl;
        }
        vector.Emplace_Back(3);
        vector.Insert(vector.Begin(), number);
        auto it = vector.Begin();
        vector.Insert(++it, 2);
        vector.Emplace_Back(4);
        vector.Emplace_Back(5);
        vector.Push_Back(6);
        
        for (size_t i = 0, I = vector.Size(); i < I; ++i)
            vector[i] = (int)i;
        for (auto it = Begin(vector); it != vector.End(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        std::cout << "Vector: reverse" << std::endl;
        for (auto it = vector.RBegin(); it != vector.REnd(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        
        [[maybe_unused]] auto data = vector.Data();
        [[maybe_unused]] auto find = std::find(Begin(vector), End(vector), 3);
        
        [[maybe_unused]] auto front = vector.Front();
        [[maybe_unused]] auto back = vector.Back();
        [[maybe_unused]] auto compare1 = vector.Begin() == vector.End();
        [[maybe_unused]] auto compare2 = vector.Begin() != vector.End();
        vector.Swap(vector);
        vector = vector;
        vector = std::move(vector);
        Custom_Vector<int> vector2(vector);
        Custom_Vector<int> vector3;
        vector3 = vector;
        Custom_Vector<int> vector4(std::move(vector2));
        Custom_Vector<int> vector5;
        vector5 = std::move(vector3);
        vector4.Swap(vector5);
        Custom_Vector<int> vector6 = {1, 2, 3};
        auto vector7 = vector6;
        vector6 == vector7;
        [[maybe_unused]] auto compare3 = (vector6 == vector7);
        [[maybe_unused]] auto compare4 = (vector6 != vector7);
        vector6.At(1) = 5;
        [[maybe_unused]] auto compare5 = (vector6 == vector7);
        [[maybe_unused]] auto compare6 = (vector6 != vector7);
        vector.Fill(6);
        
        std::cout << "Vector: Fill" << std::endl;
        for (auto it = vector.CBegin(); it != vector.CEnd(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        
        vector.Erase(vector.Begin());
        std::cout << "Vector: erase" << std::endl;
        for (auto it = vector.CBegin(); it != vector.CEnd(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        vector.Erase(vector.Begin(), vector.End());
        std::cout << "Vector: erase" << std::endl;
        for (auto it = vector.CBegin(); it != vector.CEnd(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        
        Custom_Vector<Example> examples(3);
        examples[0] = Example(1, "number1");
        examples[1] = Example(2, "number2");
        examples[2] = Example(3, "number3");
        [[maybe_unused]] auto emplace_front = examples.Emplace(examples.End(), 1, "number4");
        
        std::cout << "list example: " << std::endl;
        for (auto it = examples.Begin(); it != examples.End(); ++it)
            std::cout << *it;
        std::cout << std::endl;
        
        Custom_Vector<Example> examples_copy = examples;
        examples_copy[0] = Example(4, "number5");
        examples_copy[1] = Example(5, "number6");
        examples_copy[2] = Example(6, "number7");
        examples_copy[3] = Example(6, "number8");
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
